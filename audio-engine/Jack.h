#ifndef JACK_H
#define JACK_H
#include <string>

#include "Buffer.h"
class Module;

class Jack {
  // A jack is a port on a module that can function as an input or an output
 public:
  Jack(Module& m, int i, std::string _name) : module(m), id(i), name(_name) {}
  Module& module;                   // reference to owning module
  Buffer<float>* buffer = nullptr;  // i/o buffer
  std::string name;                 // human readable name of jack
  int id;                           // index in module's list
  bool connected = false;           // if jack is connected to something
};

class InputJack : public Jack {
  using Jack::Jack;
};

class OutputJack : public Jack {
  using Jack::Jack;
};
#endif