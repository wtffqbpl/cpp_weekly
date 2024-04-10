#include <gtest/gtest.h>
#include "internal_check_conds.h"
#include <tuple>

namespace {

template <typename... T> struct Group;

template <typename T1> struct Group<T1> {
  T1 t1_;

  Group() = default;
  explicit Group(const T1& t1) : t1_(t1) {}
  explicit Group(T1&& t1) : t1_(std::move(t1)) {}

  virtual explicit operator const T1&() const { return t1_; }
  virtual explicit operator T1&() { return t1_; }
};

template <typename T1, typename... T>
struct Group<T1, T...> : Group<T...> {
  T1 t1_;

  Group() = default;
  explicit Group(const T1& t1, T&& ...t)
      : Group<T...>(std::forward<T>(t)...)
        , t1_(t1) {}

  explicit Group(T1&& t1, T&& ...t)
      : Group<T...>(std::forward<T>(t)...)
        , t1_(std::move(t1)) {}

  explicit operator const T1&() const { return t1_; }
  explicit operator T1&() { return t1_; }
};

template <typename... T> Group(T&&... t) -> Group<T...>;

int example1() {

  Group g{3, 2.2, std::string("xyz")};

#define print_expr(expr) \
  std::cout << #expr << " = " << (expr) << std::endl;

  print_expr(int(g));
  print_expr(double(g));
  print_expr(std::string(g));

  return 0;
}

}

TEST(variadic_class_inheritance, example1) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  example1();

  auto act_output = testing::internal::GetCapturedStdout();

  oss << "int(g) = 3\n"
         "double(g) = 2.2\n"
         "std::string(g) = xyz\n";

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

namespace {

template <typename T>
class AddSpace {
private:
  const T &ref_;

public:
  explicit AddSpace(const T &ref) : ref_{ref} {}

  friend std::ostream &operator<<(std::ostream &os, const AddSpace<T> &item) {
    return os << item.ref_ << ", ";
  }
};

template <auto... Values> struct ValueList {};

template <auto... Values>
void print(const ValueList<Values...> &/* value_list */) {
  (std::cout << ... << AddSpace(Values)) << '\n';
}

void example2() {
  auto value_list = ValueList<1, 2, 3, 4, 5, 6>{};

  print(value_list);
}

}

TEST(variadic_class_inheritance, example2) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  example2();

  auto act_output = testing::internal::GetCapturedStdout();

  oss << "1, 2, 3, 4, 5, 6, \n";

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

namespace {

template <typename... F> struct overload_set;

template <typename F1> struct overload_set<F1> : public F1 {
  overload_set(F1 &&f1) : F1(std::move(f1)) {}
  overload_set(const F1 &f1) : F1(f1) {}
  using F1::operator();
};

template <typename F1, typename... F>
struct overload_set<F1, F...> : public F1, public overload_set<F...> {
  overload_set(F1 &&f1, F &&...f) : F1(std::move(f1)), overload_set<F...>(std::forward<F>(f)...) {}
  overload_set(const F1 &f1, F &&...f) : F1(f1), overload_set<F...>(std::forward<F>(f)...) {}

  using F1::operator();
  using overload_set<F...>::operator();
};

template <typename... F> auto overload(F &&...f) {
  return overload_set<F...>(std::forward<F>(f)...);
}

// In C++17, we can do better, and it gives us a chance to demonstrate an alternative way of using
// a parameter pack that does not need recursion.

template <typename... F> struct overload_set_17 : public F... {
  overload_set_17(F &&...f) : F(std::forward<F>(f))... {}

  using F::operator()...; // C++17
};

// With deduction guide, there's no need to
template <typename... F> auto overload_17(F &&...f) {
  return overload_set<F...>(std::forward<F>(f)...);
}

// Deduction guide for this overload_set
template <typename... F> overload_set_17(F &&...) -> overload_set_17<F...>;

void example3() {
  int i = 5;
  double d = 7.3;

  auto l = overload_set_17{[](int *i) { std::cout << "i = " << *i << std::endl; },
                           [](double *d) { std::cout << "d = " << *d << std::endl; }};

  l(&i);
  l(&d);
}

} // namespace

TEST(variadic_class_inheritance, example3) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  example3();

  oss << "i = 5\n"
         "d = 7.3\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}