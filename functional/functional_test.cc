#include "my_timer.h"
#include <cmath>
#include <functional>
#include <gtest/gtest.h>
#include <iostream>

// std::function
// 虽然很有用，但是因为它有明显的性能问题，为了隐藏包含的类型并提供一个对
// 所有可调用类型的通用接口，std::function 使用类型擦除(type
// erasure)的技术,该技术基于
// 虚成员函数调用，因为虚函数在运行时进行的，编译器不能在线调用，因此失去了优化的机会。

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

double add(double x, double y) { return x + y; }

TEST(functional, basic_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  using bin_func = std::function<double(double, double)>;
  bin_func f = &add;

  std::cout << "f(6, 3) = " << f(6, 3) << std::endl;
  oss << "f(6, 3) = 9\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

inline double sub(double x, double y) { return x - y; }

// support inline function. 获取地址是通过隐式获取到的
inline double sub_inline(double x, double y) { return x - y; }

// Support functor.
struct mult {
  double operator()(double x, double y) const { return x * y; }
};

template <typename T> struct power {
  T operator()(T x, T y) const { return std::pow(x, y); }
};

struct greater_t {
  template <typename T> T operator()(T x, T y) const { return x > y; }
} greater_than;

TEST(functional, supported_types) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  using bin_func = std::function<double(double, double)>;
  std::vector<bin_func> functions;

  functions.emplace_back(&add);
  functions.emplace_back(&sub);
  functions.emplace_back(&sub_inline);
  functions.emplace_back(mult{});
  functions.emplace_back(power<double>{});
  functions.emplace_back(greater_than);
  functions.emplace_back([](double x, double y) -> double { return x * y; });

  for (auto &func : functions)
    std::cout << func(10.f, 3.f) << std::endl;
  std::string act_output = testing::internal::GetCapturedStdout();

  oss << "13\n7\n7\n30\n1000\n1\n30\n";

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}
