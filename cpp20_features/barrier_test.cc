#include <barrier>
#include <cmath>
#include <format>
#include <gtest/gtest.h>
#include <iostream>
#include <stop_token>
#include <thread>
#include <vector>

namespace {

void barrier_test() {
  // initialize and print a collection of floating-point values:
  std::vector values{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};

  // define a lambda function that prints all values.
  // -NOTE: has to be noexcept to be used as barrier callback
  auto print_values = [&values]() noexcept {
    for (auto val : values) {
      std::cout << std::format("{:<7.5}", val);
    }
    std::cout << '\n';
  };

  // print initial values:
  print_values();

  // initialize a barrier that prints the values when all threads have done
  // their computations:
  std::barrier all_done{
      int(values.size()), // initial value of the counter
      print_values};      // callback to call whenever the counter is 0

  // initialize a thread for each value to compute its square root in a loop:
  std::vector<std::thread> threads;
  threads.reserve(values.size());

  for (auto &value : values) {
    threads.emplace_back([&value, &values, &all_done] {
      // repeatedly:
      for (int i = 0; i < 5; ++i) {
        // compute square root:
        value = std::sqrt(value);
        // and synchronize with other threads to print values:
        all_done.arrive_and_wait();
      }
    });
  }

  for (auto &t : threads)
    t.join();
}

} // namespace

TEST(barrier_test, basic_test) { barrier_test(); }