#ifndef BUFFER_H
#define BUFFER_H
#define DEFAULT_BUFFER_SIZE 512
#include <algorithm>
#include <memory>

template <typename T>
class Buffer {
 public:
  Buffer();
  Buffer(int size);
  Buffer(const Buffer& rhs);
  Buffer& operator=(const Buffer& rhs);

  Buffer copy() const;
  void free();
  void alloc();

  ~Buffer();

  int getSize() const;
  void setSize(int);
  T& operator[](int index);
  const T& operator[](int index) const;
  T& at(int index);
  T* getPointer();

  bool inUse = true;

 private:
  T* data;
  int size;
};

template <typename T>
class RingBuffer {
  // Provides an interface to a Buffer for circular/ring buffers
 public:
  RingBuffer(int size);

  void push(T data);
  T get(int delay) const; // delay < buf.getSize()

  int size() const;
 private:
  Buffer<T> buf;
  int head = 0;
};

#include "Buffer.cpp"

#endif