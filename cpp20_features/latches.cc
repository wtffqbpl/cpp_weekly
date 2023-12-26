#include <gtest/gtest.h>
#include <array>
#include <thread>
#include <latch>

namespace {
using namespace std::literals; // for duration literals

void loop_over(char c) {
  // loop over printing the char c:
  for (int j = 0; j < c / 2; ++j) {
    std::cout.put(c).flush();
    std::this_thread::sleep_for(100ms);
  }
}

void latch_basic_test() {
  std::array tags{'.', '?', '8', '+', '-'}; // tags we have to perform a task for

  // initialize latch to react when all tasks are done:
  std::latch all_done{tags.size()}; // initialize countdown with number of tasks.

  // start two threads dealing with every second tag:
  std::thread t1{[tags, &all_done] {
    for (unsigned  i = 0; i < tags.size(); i += 2) { // even indexes
      loop_over(tags[i]);
      // signal that the task is done:
      all_done.count_down(); // atomically decrement counter of latch.
    }
  }};

  std::thread t2{[tags, &all_done] {
    for (unsigned  i = 1; i < tags.size(); i += 2) { // odd indexes
      loop_over(tags[i]);
      // signal that the task is done:
      all_done.count_down(); // atomically decrement counter of latch.
    }
  }};

  // wait until all tasks are done:
  std::cout << "\nwaiting untill all tasks are done\n";
  all_done.wait(); // wait until counter of latch is zero
  std::cout << "\nall tasks done\n"; // note: threads might still run

  t1.join();
  t2.join();
}
}

TEST(latch_test, basic_test) {
  latch_basic_test();
}