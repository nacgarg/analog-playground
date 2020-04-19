#ifndef AUDIOGRAPH_H
#define AUDIOGRAPH_H
#include <memory>
#include <unordered_map>
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

  Buffer<float>* allocateBuffer() {
    // TODO: reuse orphaned buffers
    Buffer<float>* b = new Buffer<float>(bufferSize);
    allocatedBuffers.push_back(b);
    return b;
  }

 private:
  int bufferSize = 2048;
  void traverse(Module*, bool);

  std::vector<Buffer<float>*> allocatedBuffers;
};

#endif