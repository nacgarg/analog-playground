#include <iostream>

#include "AudioGraph.h"
#include "Log.h"
#include "Wav.h"

int main() {
  int numSamples = 256 * 5000;
  int bufferSize = 256;

  Log::setLevel(LogLevel::INFO);
  AudioGraph graph(bufferSize);
  auto noise = graph.addModule<NoiseModule>(0.5);
  auto delayAmt = graph.addModule<ConstModule>(100);
  auto delay = graph.addModule<DelayModule>();
  auto combMix = graph.addModule<MixModule>();
  auto split = graph.addModule<SplitModule>();
  auto lfo = graph.addModule<SinOscModule>(70.0, 0.2);
  auto sum = graph.addModule<AddModule>();
  auto lfoSplit = graph.addModule<SplitModule>();
  auto sinTest = graph.addModule<SinOscModule>(0.7, 440);
  auto mult = graph.addModule<MultModule>();
  auto saw = graph.addModule<SawOscModule>(1.0, 660);
  auto moreSum = graph.addModule<AddModule>();
  auto baseFreq = graph.addModule<ConstModule>(400);
  auto square = graph.addModule<SquareOscModule>(0.8, 1000.0);

  auto mix = graph.addModule<MixModule>();

  graph.connect(lfoSplit->outputA, delay->delayInSamples);
  graph.connect(noise->noise_out, split->input);
  graph.connect(split->outputA, combMix->inputA);
  graph.connect(split->outputB, delay->input);
  graph.connect(delay->output, combMix->inputB);
  graph.connect(delayAmt->output, sum->inputA);
  graph.connect(lfo->output, sum->inputB);
  graph.connect(sum->output, lfoSplit->input);
  graph.connect(lfoSplit->outputB, sinTest->frequency);
  graph.connect(sinTest->output, mult->inputA);
  graph.connect(saw->output, mult->inputB);
  graph.connect(mult->output, mix->inputA);
  graph.connect(combMix->output, mix->inputB);
  graph.connect(baseFreq->output, moreSum->inputA);
  graph.connect(square->output, moreSum->inputB);
  graph.connect(moreSum->output, saw->frequency);
  graph.evaluate(mix);

  Log::setLevel(LogLevel::WARN);

  Buffer<float> output(numSamples);
  for (int i = 0; i < (numSamples / bufferSize); ++i) {
    graph.evaluate(mix);
    std::copy(mix->output->buffer->getPointer(),
              mix->output->buffer->getPointer() + bufferSize,
              output.getPointer() + (bufferSize * i));
  }

  writeToWav(output, "out.wav", 44100);

  return 0;
}