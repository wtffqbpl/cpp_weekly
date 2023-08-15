#include "internal_check_conds.h"
#include <gtest/gtest.h>

namespace variadic_template_demos {

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }

template <typename... Args> void foo(Args (*...args)(int, int)) {
  int tmp[] = {(std::cout << args(7, 11) << std::endl, 0)...};
}

bool test_function_types() {
  testing::internal::CaptureStdout();
  std::stringstream oss;

  foo(add, sub);

  oss << "18\n"
         "-4\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif
  return oss.str() == act_output;
}

template <typename... Args> class Derived : public Args... {
public:
  explicit Derived(const Args &...args) : Args(args)... {}
};

class Base1 {
public:
  Base1() = default;
  Base1(const Base1 &) { std::cout << "copy ctor Base1" << std::endl; }
};

class Base2 {
public:
  Base2() = default;
  Base2(const Base2 &) { std::cout << "copy ctor Base2" << std::endl; }
};

bool test_variadic_template_inherit() {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  Base1 b1;
  Base2 b2;
  Derived<Base1, Base2> d(b1, b2);

  oss << "copy ctor Base1\n"
         "copy ctor Base2\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  return oss.str() == act_output;
}

} // namespace variadic_template_demos

TEST(variadic_template_demos, function_types) {
  EXPECT_TRUE(variadic_template_demos::test_function_types());
}

TEST(variadic_template_demos, class_inherit) {
  EXPECT_TRUE(variadic_template_demos::test_variadic_template_inherit());
}