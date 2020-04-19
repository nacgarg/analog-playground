#include "Buffer.h"

template <typename T>
Buffer<T>::Buffer() : size(DEFAULT_BUFFER_SIZE) {
  alloc();
}

template <typename T>
Buffer<T>::Buffer(int _size) : size(_size) {
  alloc();
}

template <typename T>
Buffer<T>::Buffer(const Buffer& rhs) : size(rhs.size) {
  freed = false;
  data = new T[rhs.size];
  std::copy(rhs.data, rhs.data + rhs.size, data);
}

template <typename T>
Buffer<T>& Buffer<T>::operator=(const Buffer& rhs) {
  size = rhs.size;
  std::copy(rhs.data, rhs.data + rhs.size, data);
  return *this;
}

template <typename T>
Buffer<T> Buffer<T>::copy() const {
  Buffer b(*this);
  return b;
}

template <typename T>
Buffer<T>::~Buffer<T>() {
  free();
}

template <typename T>
void Buffer<T>::free() {
  if (freed) return;
  freed = true;
  delete[] data;
}

template <typename T>
void Buffer<T>::alloc() {
  freed = false;
  data = new T[size];
}

template <typename T>
int Buffer<T>::getSize() const {
  return size;
}

template <typename T>
void Buffer<T>::setSize(int _size) {
  if (size == _size) return;
  size = _size;
  alloc();
}

template <typename T>
T& Buffer<T>::operator[](int i) {
  return data[i];
}

template <typename T>
const T& Buffer<T>::operator[](int i) const {
  return data[i];
}

template <typename T>
T& Buffer<T>::at(int i) {
  return data[i];
}

template <typename T>
T* Buffer<T>::getPointer() {
  return data;
}
