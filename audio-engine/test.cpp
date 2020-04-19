#include <iostream>

#include "AudioGraph.h"
#include "Log.h"

int main() {
  Log::setLevel(LogLevel::INFO);
  AudioGraph graph;
  auto noise = graph.addModule<NoiseModule>(0.5);

  for (auto a : graph.modules) {
    std::cout << a->name << "\n";
  }

  // graph.connect(mod0->output, mod1->input2); // for checking cycle detection
  graph.evaluate(noise);

  std::cout << noise->noise_out->buffer->at(1023) << "\n";

  return 0;
}