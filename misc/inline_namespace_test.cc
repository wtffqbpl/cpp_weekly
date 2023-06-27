#include "internal_check_conds.h"
#include <gtest/gtest.h>

namespace {

namespace Parent {

namespace Child1 {
void foo() { std::cout << "Child1::foo()" << std::endl; }
} // namespace Child1

inline namespace Child2 {
void foo() { std::cout << "Child2::foo()" << std::endl; }
} // namespace Child2
} // namespace Parent
} // namespace

TEST(inline_namespace_test, test1) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  Parent::Child1::foo();
  Parent::foo();

  oss << "Child1::foo()\n"
      << "Child2::foo()\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

namespace A::B::C {
void foo() { std::cout << "A::B::C::foo()" << std::endl; }
} // namespace A::B::C

TEST(inline_namespace_test, test2) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  A::B::C::foo();

  oss << "A::B::C::foo()\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}