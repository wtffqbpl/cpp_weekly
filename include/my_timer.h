#ifndef MY_TIMER_H
#define MY_TIMER_H

#include <chrono>
#include <iostream>
#include <string_view>

class Timer {
private:
  std::string_view Title;
  std::chrono::high_resolution_clock::time_point Start;

public:
  Timer(const std::string_view &Title_) : Title(Title_) {
    Start = std::chrono::high_resolution_clock::now();
  }

  ~Timer() { stop(); }

  /*
   * @brief time eclipse from construct this timer class with milliseconds.
   */
  uint64_t eclipses() {
    auto Stop = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(Stop - Start).count();
  }

private:
  void stop() {
    auto Stop = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(Stop - Start);
#ifndef NDEBUG
    std::cout << Title << " " << (ms.count()) * 0.001 << "s\n";
#endif
  }
};

#endif // MY_TIMER_H