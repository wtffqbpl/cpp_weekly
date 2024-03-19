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