#include <chrono>
#include <condition_variable>
#include <gtest/gtest.h>
#include <iostream>

#include "internal_check_conds.h"

namespace if_switch_with_init {

bool foo() { return true; }

void basic_test() {
  testing::internal::CaptureStdout();

  std::stringstream oss;

  if (bool b = foo(); b) {
    std::cout << std::boolalpha << "good! foo() = " << b << std::endl;
  }
  oss << "good! foo() = true\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

std::mutex mx;
bool shared_flag = true;

void thread_func() {
  if (std::lock_guard<std::mutex> lock(mx); shared_flag) {
    shared_flag = false;
  }
}

std::condition_variable cv;
std::mutex cv_m;

} // namespace if_switch_with_init

TEST(if_switch_with_init, basic_test) { if_switch_with_init::basic_test(); }