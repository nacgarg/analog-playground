#ifndef LOG_H
#define LOG_H

#include <fstream>
#include <iostream>
#include <memory>
enum LogLevel { INFO, WARN, ERROR, FATAL };

class Log {
 public:
  static LogLevel level;

  template <typename... T>
  static void log(LogLevel l, T... t) {
    if (l < level) return;

    switch (l) {
      case INFO:
        std::cout << "INFO: ";
        break;
      case WARN:
        std::cout << "WARN: ";
        break;
      case ERROR:
        std::cout << "ERROR: ";
        break;
      case FATAL:
        std::cout << "FATAL: ";
        break;
    }
    print(t...);
  }

  static void setLevel(LogLevel l) { level = l; }

 private:
  template <typename T, typename... A>
  static void print(T t, A... a) {
    std::cout << t << " ";
    print(a...);
  }

  static void print() { std::cout << std::endl; }
};

#endif