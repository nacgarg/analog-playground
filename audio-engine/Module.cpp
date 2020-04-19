#include "Module.h"

#include "Log.h"
int Module::num_created = 0;

void Module::process(int bufferSize) {
  Log::log(LogLevel::INFO, "Processing", name, "Buffer size:", bufferSize);
}

void DummyModule::process(int bufferSize) {
  Module::process(bufferSize);

  // buffers should already be the right size
  if (input->connected) {
    output->buffer = input->buffer;

    for (int i = 0; i < bufferSize; ++i) {
      output->buffer->at(i) = input->buffer->at(i) + 1;
    }
  } else {
    for (int i = 0; i < bufferSize; ++i) {
      output->buffer->at(i) = 3;
    }
  }
}