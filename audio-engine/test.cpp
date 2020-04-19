#include <iostream>

#include "AudioGraph.h"
#include "Log.h"

int main() {
  Log::setLevel(LogLevel::INFO);
  AudioGraph graph;
  auto mod0 = graph.addModule<DummyModule>();
  auto mod1 = graph.addModule<DummyModule>();
  auto mod2 = graph.addModule<DummyModule>();
  auto mod3 = graph.addModule<DummyModule>();
  auto mod4 = graph.addModule<DummyModule>();

  for (auto a : graph.modules) {
    std::cout << a->name << "\n";
  }

  graph.connect(mod1->output, mod0->input);
  graph.connect(mod2->output2, mod0->input2);
  graph.connect(mod2->output, mod1->input);
  graph.connect(mod3->output, mod2->input);
  graph.connect(mod4->output, mod2->input2);

  // graph.connect(mod0->output, mod1->input2); // for checking cycle detection
  graph.evaluate(mod0);

  std::cout << mod0->output->buffer->at(1023) << "\n";

  return 0;
}