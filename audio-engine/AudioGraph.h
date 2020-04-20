#ifndef AUDIOGRAPH_H
#define AUDIOGRAPH_H
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Log.h"
#include "Module.h"

class AudioGraph {
 public:
  AudioGraph(){};
  AudioGraph(int _bufferSize) : bufferSize(_bufferSize){};

  ~AudioGraph();

  template <typename T, typename... A>
  T* addModule(A... args) {
    Module* m(new T(args...));
    m->graph = this;
    modules.push_back(m);
    return static_cast<T*>(m);
  }

  Wire& connect(OutputJack*, InputJack*);
  void evaluate(Module*);

  std::vector<Module*> modules;
  std::vector<Wire> connections;
  std::unordered_map<OutputJack*, InputJack*> connectionMap;
  std::unordered_map<InputJack*, OutputJack*> connectionMapInv;

  Buffer<float>* allocateBuffer() {
    if (emptyBuffers > 0) {
      for (int i = 0; i < allocatedBuffers.size(); ++i) {
        Buffer<float>* empty = allocatedBuffers[i];
        if (!empty->inUse) {
          empty->inUse = true;
          emptyBuffers--;
          return empty;
        }
      }
    }

    Buffer<float>* b = new Buffer<float>(bufferSize);
    allocatedBuffers.push_back(b);
    return b;
  }

  bool freeBuffer(Buffer<float>* b) {
    if (!b->inUse) return false;
    if (emptyBuffers < 8) {
      b->inUse = false;
      emptyBuffers++;
      return false;
    }
    for (auto it = allocatedBuffers.begin(); it != allocatedBuffers.end(); ++it) {
      if (*it == b) {
        delete *it;
        allocatedBuffers.erase(it);
        return true;
      }
    }
    return false;
  }

  int bufferSize = 2048;

 private:
  void traverse(Module*, bool);
  bool changed = true;

  int emptyBuffers = 0;
  std::vector<Buffer<float>*> allocatedBuffers;
  std::vector<Module*> processOrder;
};

#endif