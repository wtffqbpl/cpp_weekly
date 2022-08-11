#ifndef MY_TIMER_H
#define MY_TIMER_H

#include <chrono>
#include <iostream>
#include <string_view>

class Timer {
  std::string_view Title;
  std::chrono::high_resolution_clock::time_point Start;
  std::chrono::high_resolution_clock::time_point Stop;

public:
  Timer(const std::string_view &Title_) : Title(Title_) {
    Start = Stop = std::chrono::high_resolution_clock::now();
  }

  ~Timer() { stop(); }

  uint64_t timeEclipses() {
    Stop = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(Stop - Start).count();
  }

private:
  void stop() {
    if (Stop == Start)
      Stop = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(Stop - Start);
#ifndef NDEBUG
    std::cout << Title << " " << (ms.count()) * 0.001 << "s\n";
#endif
  }
};

#endif // MY_TIMER_H