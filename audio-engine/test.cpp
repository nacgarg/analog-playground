#include <iostream>

#include "AudioGraph.h"
#include "Log.h"
#include "Wav.h"

int main() {
  int numSamples = 256 * 5000;
  int bufferSize = 256;

  Log::setLevel(LogLevel::INFO);
  AudioGraph graph(bufferSize);
  auto noise = graph.addModule<NoiseModule>(0);
  auto delayAmt = graph.addModule<ConstModule>(100);
  auto delay = graph.addModule<DelayModule>();
  auto combMix = graph.addModule<MixModule>();
  auto split = graph.addModule<SplitModule>();
  auto lfo = graph.addModule<SinOscModule>(70.0, 0.2);
  auto sum = graph.addModule<AddModule>();
  auto lfoSplit = graph.addModule<SplitModule>();
  auto sinTest = graph.addModule<SinOscModule>(0.7, 440);
  auto mult = graph.addModule<MultModule>();
  auto saw = graph.addModule<SawOscModule>(1.0, 55);
  auto moreSum = graph.addModule<AddModule>();
  auto baseFreq = graph.addModule<ConstModule>(400);
  auto square = graph.addModule<SquareOscModule>(0.8, 1000.0);
  auto filter = graph.addModule<MoogFilterModule>(1000, 0.2f);
  auto filterLfo = graph.addModule<SawOscModule>(2200, 3);
  auto filterLfoDC = graph.addModule<ConstModule>(2500);
  auto filterLfoSum = graph.addModule<AddModule>();

  auto mix = graph.addModule<MixModule>();

  graph.connect(lfoSplit->outputA, delay->delayInSamples);
  graph.connect(noise->noise_out, split->input);
  graph.connect(split->outputA, combMix->inputA);
  graph.connect(split->outputB, delay->input);
  graph.connect(delay->output, combMix->inputB);
  graph.connect(delayAmt->output, sum->inputA);
  graph.connect(lfo->output, sum->inputB);
  graph.connect(sum->output, lfoSplit->input);
//   graph.connect(lfoSplit->outputB, saw->frequency);
//   graph.connect(sinTest->output, saw->frequency);
  graph.connect(saw->output, mix->inputA);
//   graph.connect(mult->output, mix->inputA);
  graph.connect(combMix->output, mix->inputB);
  graph.connect(baseFreq->output, moreSum->inputA);
  graph.connect(square->output, moreSum->inputB);
//   graph.connect(moreSum->output, saw->frequency);
  graph.connect(mix->output, filter->input);
  graph.connect(filterLfo->output, filterLfoSum->inputA);
  graph.connect(filterLfoDC->output, filterLfoSum->inputB);
  graph.connect(filterLfoSum->output, filter->frequency);
  graph.evaluate(filter);

  Log::setLevel(LogLevel::WARN);

  Buffer<float> output(numSamples);
  for (int i = 0; i < (numSamples / bufferSize); ++i) {
    graph.evaluate(filter);
    auto ptr = filter->outputLow->buffer->getPointer();
    std::copy(ptr, ptr + bufferSize, output.getPointer() + (bufferSize * i));
  }

  writeToWav(output, "out.wav", graph.sampleRate);

  return 0;
}