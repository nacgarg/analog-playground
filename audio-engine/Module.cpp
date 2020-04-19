#include "Module.h"

#include "AudioGraph.h"
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

void ConstModule::process(int bufferSize) {
  Module::process(bufferSize);
  for (int i = 0; i < bufferSize; ++i) {
    output->buffer->at(i) = value;
  }
}

void NoiseModule::process(int bufferSize) {
  Module::process(bufferSize);
  for (int i = 0; i < bufferSize; ++i) {
    noise_out->buffer->at(i) = dist(rng);
  }
}

void DelayModule::process(int bufferSize) {
  Module::process(bufferSize);
  int delayLength = int(delayInSamples->buffer->at(0));

  output->buffer = input->buffer;  // reuse input buffer to store output
  delayInSamples->buffer->free();  // don't need this buffer any more

  for (int i = 0; i < bufferSize; ++i) {
    fifo.push(input->buffer->at(i));
  }

  int i = 0;
  while (fifo.size() < delayLength + bufferSize - i) {
    output->buffer->at(i++) = 0;
  }
  while (i < bufferSize) {
    output->buffer->at(i++) = fifo.front();
    fifo.pop();
  }
}

void MixModule::process(int bufferSize) {
  Module::process(bufferSize);
  output->buffer = inputA->buffer;  // reuse buffer
  for (int i = 0; i < bufferSize; ++i) {
    output->buffer->at(i) += inputB->buffer->at(i);
    output->buffer->at(i) /= 2.0;
  }
}

void SplitModule::process(int bufferSize) {
  Module::process(bufferSize);
  outputA->buffer = input->buffer;  // reuse buffer
  outputB->buffer = graph->allocateBuffer();
  for (int i = 0; i < bufferSize; ++i) {
    outputB->buffer->at(i) = outputA->buffer->at(i);
  }
}