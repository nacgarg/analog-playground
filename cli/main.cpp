#include <portaudio.h>

#include <iostream>

#include "../audio-engine/AudioGraph.h"
struct CallbackInfo {
  AudioGraph* g;
  OutputJack* output;
};
int paCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
               const PaStreamCallbackTimeInfo* timeInfo,
               PaStreamCallbackFlags statusFlags, void* userData) {
  auto cb = (CallbackInfo*)userData;
  auto g = cb->g;
  float* out = (float*)outputBuffer;
  g->evaluate(&cb->output->module);
  for (int i = 0; i < framesPerBuffer; i++) {
    out[i] = cb->output->buffer->at(i);
  }
  return paContinue;
}

int main() {
  AudioGraph g(1024);
  g.sampleRate = 44100;
  Log::setLevel(LogLevel::ERROR);

  auto noise = g.addModule<NoiseModule>(0.5);

  PaError err = Pa_Initialize();
  if (err != paNoError) {
    std::cerr << "Failed to initialize PortAudio\n";
    return 1;
  }

  PaStreamParameters outputParameters;

  auto deviceIndex = Pa_GetDefaultOutputDevice();
  outputParameters.device = deviceIndex;
  const PaDeviceInfo* pInfo = Pa_GetDeviceInfo(deviceIndex);

  std::cout << "Output device: " << pInfo->name << std::endl;

  PaStream* stream;
  CallbackInfo cb{&g, noise->noise_out};
  err = Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, g.sampleRate, g.bufferSize,
                             &paCallback, &cb);
  if (err != paNoError) {
    std::cerr << "Failed to open PortAudio stream\n";
    return 1;
  }
  err = Pa_StartStream(stream);
  if (err != paNoError) {
    std::cerr << "Failed to start PortAudio stream\n";
    return 1;
  }
  while (true) {
    if (!std::cin) continue;
    std::cout << ">> ";
    std::string command;
    std::cin >> command;
    if (command == "modules") {
      for (const auto& mod : g.modules) {
        std::cout << "   " << mod->id << ": " << mod->name << "\n";
      }
    }
    if (command == "jacks") {
      int moduleId;
      std::cin >> moduleId;
      if (moduleId >= g.modules.size()) {
        continue;
      }
      std::cout << "   inputs\n";
      for (const auto& jack : g.modules[moduleId]->inputs) {
        std::cout << "    " << (char)('a' + jack->id) << ": " << jack->name << "\n";
      }
      std::cout << "   outputs\n";
      for (const auto& jack : g.modules[moduleId]->outputs) {
        std::cout << "    " << (char)('a' + jack->id) << ": " << jack->name << "\n";
      }
    }
    if (command == "add") {
      std::string type;
      std::cin >> type;
      if (type == "noise") g.addModule<NoiseModule>();
      if (type == "const") g.addModule<ConstModule>();
      if (type == "delay") g.addModule<DelayModule>();
      if (type == "mix") g.addModule<MixModule>();
      if (type == "split") g.addModule<SplitModule>();
      if (type == "sine") g.addModule<SinOscModule>();
      if (type == "mult") g.addModule<MultModule>();
      if (type == "saw") g.addModule<SawOscModule>();
      if (type == "add") g.addModule<AddModule>();
      if (type == "square") g.addModule<SquareOscModule>();
      if (type == "filter") g.addModule<MoogFilterModule>();
      auto mod = g.modules.back();
      std::cout << "   " << mod->id << ": " << mod->name << "\n";
      std::cout << "   inputs\n";
      for (const auto& jack : mod->inputs) {
        std::cout << "    " << (char)('a' + jack->id) << ": " << jack->name << "\n";
      }
      std::cout << "   outputs\n";
      for (const auto& jack : mod->outputs) {
        std::cout << "    " << (char)('a' + jack->id) << ": " << jack->name << "\n";
      }
    }
    if (command == "connect") {
      int moduleIdA;
      int moduleIdB;
      char jackA;
      char jackB;
      std::cin >> moduleIdA;
      std::cin >> jackA;
      std::cin >> moduleIdB;
      std::cin >> jackB;
      g.connect(g.modules[moduleIdA]->outputs[jackA - 'a'],
                g.modules[moduleIdB]->inputs[jackB - 'a']);
    }
    if (command == "output") {
      int moduleId;
      std::cin >> moduleId;
      char jack;
      std::cin >> jack;
      cb.output = g.modules[moduleId]->outputs[jack - 'a'];
    }
  }
  return 0;
}