#ifndef WAV_H
#define WAV_H
#include <fstream>
#include <iostream>
#include <string>

#include "Buffer.h"
#include "math.h"

void writeToWav(const Buffer<float>& b, const std::string& path, int sampleRate) {
  std::ofstream file(path);

  Log::log(LogLevel::INFO, "Writing wav file to", path, "samples:", b.getSize());

  int bitDepth = 16;
  int sampleSize = bitDepth / 8;
  int channels = 1;

  file.write("RIFF", 4);
  int fileSize = 44 + (sampleSize * b.getSize()) - 8;
  file.write((const char*)&fileSize, 4);
  file.write("WAVE", 4);
  file.write("fmt ", 4);
  int formatLength = 16;
  file.write((const char*)&formatLength, 4);
  short formatType = 1;
  file.write((const char*)&formatType, 2);
  file.write((const char*)&channels, 2);
  file.write((const char*)&sampleRate, 4);
  int mult = sampleRate * bitDepth * channels / 8;
  file.write((const char*)&mult, 4);
  mult = bitDepth * channels;
  file.write((const char*)&mult, 2);
  mult = bitDepth;
  file.write((const char*)&mult, 2);
  file.write("data", 4);
  int dataSize = (sampleSize * b.getSize());
  file.write((const char*)&fileSize, 4);

  for (int i = 0; i < b.getSize(); ++i) {
    float val = b[i];
    int converted = val * (1 << (bitDepth - 1));
    file.write((const char*)&converted, sampleSize);
  }

  file.close();
}

#endif