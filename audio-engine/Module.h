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

  AudioGraph* graph;  // reference to graph that this belongs to

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

class AddModule : public Module {
 public:
  AddModule() : Module("AddModule") {
    output = addOutputJack("Sum Output");
    inputA = addInputJack("Add Input A");
    inputB = addInputJack("Add Input B");
  }
  OutputJack* output;
  InputJack* inputA;
  InputJack* inputB;

  void process(int bufferSize) override;
};

class MultModule : public Module {
 public:
  MultModule() : Module("MultModule") {
    output = addOutputJack("Multipled Output");
    inputA = addInputJack("Multiply Input A");
    inputB = addInputJack("Multiply Input B");
  }
  OutputJack* output;
  InputJack* inputA;
  InputJack* inputB;

  void process(int bufferSize) override;
};

class NCOModule : public Module {
 public:
  NCOModule(const std::string& name = "NCOModule", float _a = 1.0, float _f = 440)
      : Module(name), a(_a), f(_f) {
    output = addOutputJack("NCO Output");
    amplitude = addInputJack("Amplitude");
    frequency = addInputJack("Frequency");
  }
  virtual ~NCOModule() {};
  OutputJack* output;
  InputJack* amplitude; 
  InputJack* frequency;

  void process(int bufferSize) override;

 protected:
  virtual void calculate_lut() = 0;

  float a;
  float f;
  static const int lut_qual = 10;
  static const int lut_len = 1 << lut_qual;
  float lut[lut_len];
  unsigned phase = 0;
  bool lut_calculated = false;
  const float ONE_ROTATION = 2.0 * (1u << (sizeof(unsigned) * 8 - 1));
};

class SinOscModule : public NCOModule {
 public:
  SinOscModule(float _a = 1.0, float _f = 440.0) : NCOModule("SinOscModule", _a, _f) {}

 protected:
  void calculate_lut() override;
};

class SawOscModule : public NCOModule {
 public:
  SawOscModule(float _a = 1.0, float _f = 440.0) : NCOModule("SawOscModule", _a, _f) {}

 protected:
  void calculate_lut() override;
};

class SquareOscModule : public NCOModule {
 public:
  SquareOscModule(float _a = 1.0, float _f = 440.0) : NCOModule("SquareOscModule", _a, _f) {}

 protected:
  void calculate_lut() override;
};


#endif