#ifndef MODULE_H
#define MODULE_H

#include <memory>
#include <queue>
#include <random>
#include <string>
#include <vector>

#include "Jack.h"

class AudioGraph;

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

  AudioGraph* graph; // reference to graph that this belongs to

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

class NoiseModule : public Module {
 public:
  NoiseModule(float gain) : Module("NoiseModule"), rng(rd()), dist(-gain, gain) {
    noise_out = addOutputJack("Noise Output");
  }
  OutputJack* noise_out;

  void process(int bufferSize) override;

 private:
  std::random_device rd;
  std::mt19937 rng;
  std::uniform_real_distribution<float> dist;
};

class ConstModule : public Module {
 public:
  ConstModule(float _value) : Module("ConstModule"), value(_value) {
    output = addOutputJack("Constant Output");
  }
  OutputJack* output;

  void process(int bufferSize) override;

 private:
  float value;
};

class DelayModule : public Module {
 public:
  DelayModule() : Module("DelayModule") {
    output = addOutputJack("Delayed Output");
    input = addInputJack("Delay Input");
    delayInSamples = addInputJack("Delay time in samples");
  }
  OutputJack* output;
  InputJack* input;
  InputJack* delayInSamples;

  void process(int bufferSize) override;

 private:
  std::queue<float> fifo;
};

class MixModule : public Module {
  public:
  MixModule() : Module("MixModule") {
    output = addOutputJack("Mixed Output");
    inputA = addInputJack("Mix Input A");
    inputB = addInputJack("Mix Input B");
  }
  OutputJack* output;
  InputJack* inputA;
  InputJack* inputB;

  void process(int bufferSize) override;
};

class SplitModule : public Module {
  public:
  SplitModule() : Module("SplitModule") {
    outputA = addOutputJack("Split Output A");
    outputB = addOutputJack("Split Output B");
    input = addInputJack("Split Input");
  }
  OutputJack* outputA;
  OutputJack* outputB;
  InputJack* input;

  void process(int bufferSize) override;
};

#endif