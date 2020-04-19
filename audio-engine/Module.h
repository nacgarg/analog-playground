#ifndef MODULE_H
#define MODULE_H

#include <memory>
#include <string>
#include <vector>

#include "Jack.h"

class Module {
  // A module can have multiple inputs and outputs

 public:
  std::string name;
  Module(const std::string& _name)
      : id(num_created++), name(_name + "_" + std::to_string(num_created)) {}
  virtual ~Module() {
    for (auto* j : inputs) delete j;
    for (auto* j : outputs) delete j;
  }

  virtual void process(int bufferSize);  // requires that all input jacks have valid
                                         // data in i/o buffers
  int id;
  static int num_created;

  std::vector<InputJack*> inputs;
  std::vector<OutputJack*> outputs;

 protected:
  template <typename... A>
  InputJack* addInputJack(A... args) {
    InputJack* j(new InputJack(*this, inputs.size(), args...));
    inputs.push_back(j);
    return j;
  }

  template <typename... A>
  OutputJack* addOutputJack(A... args) {
    OutputJack* j(new OutputJack(*this, outputs.size(), args...));
    outputs.push_back(j);
    return j;
  }
};

class Wire {
 public:
  Wire(const OutputJack* a, const InputJack* b) : from(a), to(b) {}
  const OutputJack* from;
  const InputJack* to;
};

class DummyModule : public Module {
 public:
  DummyModule() : Module("DummyModule") {
    input = addInputJack("Dummy Input");
    output = addOutputJack("Dummy Output");

    input2 = addInputJack("Dummy Input 2");
    output2 = addOutputJack("Dummy Output 2");
  }
  InputJack* input;
  OutputJack* output;

  InputJack* input2;
  OutputJack* output2;

  void process(int bufferSize) override;
};

#endif