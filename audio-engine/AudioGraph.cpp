#include "AudioGraph.h"

#include <iostream>
#include <unordered_map>
#include <unordered_set>

Wire& AudioGraph::connect(OutputJack* a, InputJack* b) {
  connections.emplace_back(a, b);
  if (a->connected) {
    Log::log(LogLevel::ERROR, "OutputJacks can only have one connection");
    throw std::invalid_argument("OutputJacks can only have one connection");
  }
  if (b->connected) {
    Log::log(LogLevel::ERROR, "InputJacks can only have one connection");
    throw std::invalid_argument("InputJacks can only have one connection");
  }
  a->connected = true;
  b->connected = true;
  connectionMap[a] = b;

  return connections.back();
}

AudioGraph::~AudioGraph() {
  for (Module* m : modules) {
    delete m;
  }
  for (auto* b : allocatedBuffers) {
    delete b;
  }
};

void AudioGraph::evaluate(Module* m) {
  // Evaluates the graph to produce a single buffer in m's output jacks
  // Find any dependencies of m

  // Ideally, this should figure out the topology of the graph and optimize memory copies
  // (serial graphs without branches should be evaluated in place)

  // Right now we are essentially processing nodes in a post-order traversal
  traverse(m, true);
}

void AudioGraph::traverse(Module* m, bool start) {
  static std::unordered_set<const Jack*> visited;
  static std::unordered_set<const Module*> processed;

  if (start) {
    visited.clear();
    processed.clear();
  }

  if (processed.find(m) != processed.end()) return;

  bool isLeaf = true;

  Log::log(LogLevel::INFO, "Checking dependencies for", m->name);
  if (!m->inputs.empty()) {
    for (auto w : connections) {
      for (auto input : m->inputs) {
        if (w.to == input) {
          visited.insert(input);
          auto v = &w.from->module;
          Log::log(LogLevel::INFO, "Found dependency:", m->name, "(" + input->name + ")",
                   "requires", w.from->name, "from", v->name);
          if (visited.find(w.from) != visited.end()) {
            Log::log(LogLevel::ERROR, "Detected cycle in graph");
            throw std::invalid_argument("Cycle in audio graph");
          }
          visited.insert(w.from);
          isLeaf = false;
          traverse(v, false);
        }
      }
    }
  }
  if (isLeaf) {
    Log::log(LogLevel::INFO, "Found leaf:", m->name);
    // Initialize memory for the leaf module
    for (OutputJack* output : m->outputs) {
      if (output->connected || start) {
        Log::log(LogLevel::INFO, "Initializing output buffer for", output->name);
        output->buffer = allocateBuffer();
      }
    }
  }
  m->process(bufferSize);
  processed.insert(m);
  for (OutputJack* output : m->outputs) {
    if (output->connected) {
      Log::log(LogLevel::INFO, "Copying buffer from", output->module.name, "to",
               connectionMap[output]->module.name);
      connectionMap[output]->buffer = output->buffer;
    }
  }
}