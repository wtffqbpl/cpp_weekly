#include "my_timer.h"
#include <gtest/gtest.h>
#include <iostream>
#include <functional>

int do_something(int i) { return 5 + i; }

struct S {
  int val = 10;
  int do_something_1(const int i) { return val + i; }
  int do_something_2(const int i) { return val * i; }
};

TEST(Functional_invoke, basic_test) {
  std::stringstream oss;
  std::string act_output;

  {
    testing::internal::CaptureStdout();
    Timer T("invoke test");
    std::cout << std::invoke(&do_something, 5) << '\n';
    oss << "10" << std::endl;

    S s;
    std::cout << s.do_something_1(3) << '\n';
    oss << "13" << std::endl;

    auto fp = &S::do_something_1;
    int (S::*fp2)(int) = nullptr;

    if (true)
      fp2 = &S::do_something_2;
    else
      fp2 = &S::do_something_1;

    std::cout << (s.*fp2)(2) << '\n';
    oss << "20" << std::endl;
    std::cout << (s.*fp2)(1) << '\n';
    oss << "10" << std::endl;

    // invoke function which is in struct case.
    std::cout << std::invoke(&S::do_something_1, s, 2) << '\n';
    oss << "12\n";
    std::cout << std::invoke(&S::do_something_2, s, 1) << '\n';
    oss << "10\n";

    act_output = testing::internal::GetCapturedStdout();
  }

#ifndef NDEBUG
  std::cout << "Actual output:\n" << act_output << std::endl;
  std::cout << "Expected output:\n" << oss.str() << std::endl;
#endif

  EXPECT_TRUE(act_output == oss.str());
}
