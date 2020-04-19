#include <iostream>

#include "AudioGraph.h"
#include "Log.h"
#include "Wav.h"

int main() {
  Log::setLevel(LogLevel::INFO);
  AudioGraph graph(44100 * 10);
  auto noise = graph.addModule<NoiseModule>(0.5);
  auto delayAmt = graph.addModule<ConstModule>(100);
  auto delay = graph.addModule<DelayModule>();
  auto mix = graph.addModule<MixModule>();
  auto split = graph.addModule<SplitModule>();

  graph.connect(delayAmt->output, delay->delayInSamples);
  graph.connect(noise->noise_out, split->input);
  graph.connect(split->outputA, mix->inputA);
  graph.connect(split->outputB, delay->input);
  graph.connect(delay->output, mix->inputB);

  graph.evaluate(mix);


  writeToWav(*mix->output->buffer, "out.wav", 44100);

  return 0;
}