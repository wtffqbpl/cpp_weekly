#if __cplusplus >= 202002L

#ifdef __APPLE__

#include <gtest/gtest.h>
#include <iostream>
#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <semaphore>
#include <stop_token>

namespace {

void semaphores_basic_test() {
  std::queue<char> values; // queue of values
  std::mutex values_mtx; // mutex to protect access to the queue

  // initialize a queue with multiple sequences from 'a' to 'z':
  for (int i = 0; i < 1000; ++i) {
    values.push(static_cast<char>('a' + (i % ('z' - 'a'))));
  }

  // create a pool of num_threads threads:
  // - limit their availability with a semaphore (initially none available):
  constexpr int num_threads = 10;
  std::counting_semaphore<num_threads> enabled{0};

  // create and start all threads of the pool:
  std::vector<std::thread> pool;
  for (int idx = 0; idx < num_threads; ++idx) {
    // std::thread t{[idx, &enabled, &values, &values_mtx] (std::stop_token st) { }};
    // pool.push_back(std::move(t));
  }
}

} // namespace

#endif // __APPLE__

#endif // __cplusplus >= 202002L
