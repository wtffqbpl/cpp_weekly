#if __cplusplus >= 202002L

#ifdef __APPLE__

#include <barrier>
#include <cmath>
#include <format>
#include <gtest/gtest.h>
#include <iostream>
#include <stop_token>
#include <thread>
#include <vector>

namespace {

// There are two differences between a std::latch and a std::barrier. First,
// you can use a std::barrier more than once, and second, you can adjust the
// counter for the next phase. The counter is set in the constructor of
// std::barrier `bar`. Calling `bar.arrive()`, `bar.arrive_and_wait()`, and
// `bar.arrive_and_drop()` decrements the counter in the current phase.
// Additionally, `bar.arrive_and_drop()` decrements the counter for the next
// phase. Immediately after the current phase is finished and the counter
// becomes zero, the so-called completion step starts. In this completion step,
// a callable is invoked. The `std::barrier` gets its callable in its
// constructor.
// The completion step performs the following steps:
//  * All threads are blocked.
//  * An arbitrary thread is unblocked and executes the callable. The callable
//    must not throw and has to be `noexcept`.
//  * If the completion step is done, all threads are unblocked.

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

// Full-time and part-time workers.

std::barrier work_done{6};
std::mutex cout_mutex;

void synchronized_out(const std::string &s) noexcept {
  std::lock_guard<std::mutex> lo(cout_mutex);
  std::cout << s;
}

class FullTimeWorker {
public:
  explicit FullTimeWorker(std::string n) : name(std::move(n)) {}

  void operator()() {
    synchronized_out(name + ": " + "Morning work done!\n");
    work_done.arrive_and_wait(); // Wait until morning work is done.
    synchronized_out(name + ": " + "Afternoon work done!\n");
    work_done.arrive_and_wait(); // Wait until afternoon work is done.
  }

private:
  std::string name;
};

class PartTimeWorker {
public:
  explicit PartTimeWorker(std::string n) : name(std::move(n)) {}

  void operator()() {
    synchronized_out(name + ": " + "Morning work done!\n");
    work_done.arrive_and_drop(); // Wait until morning work is done
  }

private:
  std::string name;
};

void work_test() {
  std::vector<std::string> full_time_worker_name_list{
      "  Herb",
      "   Scott",
      "    Bjarne",
  };
  std::vector<std::string> part_time_worker_name_list{
      "     Andrei",
      "      Andrew",
      "       David",
  };

  std::vector<std::thread> worker_list;
  worker_list.reserve(full_time_worker_name_list.size() +
                      part_time_worker_name_list.size());
  // Full-time workers
  for (const auto &name : full_time_worker_name_list)
    worker_list.emplace_back(FullTimeWorker(name));

  // Part-time workers
  for (const auto &name : part_time_worker_name_list)
    worker_list.emplace_back(PartTimeWorker(name));

  for (auto &t : worker_list)
    t.join();
}

} // namespace

TEST(barrier_test, basic_test) { barrier_test(); }

TEST(barrier_test, test2) { work_test(); }

#endif // __APPLE__

#endif // __cplusplus >= 202002L
