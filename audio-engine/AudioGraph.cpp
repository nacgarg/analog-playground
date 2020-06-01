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
  connectionMapInv[b] = a;

  changed = true;

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
  std::vector<bool> oldConnected;
  oldConnected.reserve(m->outputs.size());
  for (auto &o : m->outputs) {
    oldConnected.push_back(o->connected);
    o->connected = true;
  }
  for (int i = 0; i < allocatedBuffers.size(); ++i) {
    if (freeBuffer(allocatedBuffers[i])) i--;
  }
  if (changed || lastEval != m) traverse(m, true);
  changed = false;
  lastEval = m;
  for (Module* mod : processOrder) {
    mod->process(bufferSize);
    for (OutputJack* output : mod->outputs) {
      Log::log(LogLevel::INFO, "Checking outputs for", output->name);
      if (output->connected && mod != m) {
        auto& nextInput = connectionMap[output];
        Log::log(LogLevel::INFO, "Copying buffer from", output->module.name, "to",
                 nextInput->module.name);
        nextInput->buffer = output->buffer;
      }
    }
  }
  for (auto &o : m->outputs) {
    o->connected = oldConnected.back();
    oldConnected.pop_back();
  }
  Log::log(LogLevel::INFO, "Buffers in use after evaluate:", allocatedBuffers.size(), emptyBuffers);
}

void AudioGraph::traverse(Module* m, bool start) {
  static std::unordered_set<const Jack*> visited;
  static std::unordered_set<const Module*> processed;

  if (start) {
    visited.clear();
    processed.clear();
    processOrder.clear();
    processOrder.reserve(modules.size());
  }

  if (processed.find(m) != processed.end()) return;

  Log::log(LogLevel::INFO, "Checking dependencies for", m->name);
  if (!m->inputs.empty()) {
    for (auto input : m->inputs) {
      auto it = connectionMapInv.find(input);
      if (it == connectionMapInv.end()) continue;
      auto from = it->second;
      visited.insert(input);
      auto v = &from->module;
      Log::log(LogLevel::INFO, "Found dependency:", m->name, "(" + input->name + ")",
               "requires", from->name, "from", v->name);
      if (visited.find(from) != visited.end()) {
        Log::log(LogLevel::ERROR, "Detected cycle in graph");
        throw std::invalid_argument("Cycle in audio graph");
      }
      visited.insert(from);
      traverse(v, false);
    }
  }
  processOrder.push_back(m);
  processed.insert(m);
}