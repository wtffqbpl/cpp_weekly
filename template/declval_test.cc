#include <boost/type_index.hpp>
#include <gtest/gtest.h>
#include <type_traits>

#include "internal_check_conds.h"

namespace ns_declval_test {

struct NonDefault {
  NonDefault() = delete;

  int foo() { return 1; }
};

void declval_basic_test1() {
  using boost::typeindex::type_id_with_cvr;

  using T = decltype(std::declval<NonDefault>().foo());
  std::cout << "decltype(std::declval<NonDefault>()) = "
            << type_id_with_cvr<T>().pretty_name() << std::endl;
}

struct BaseT {
  virtual ~BaseT() = default;
};

template <typename T> struct Derived1 : public BaseT { virtual T t() = 0; };

template <typename T> struct Derived2 : public Derived1<T> {
  ~Derived2() override = default;

  T t() override {
    std::cout << "A"
              << "\n";
    return T{};
  }
};

void declval_basic_test2() {
  using boost::typeindex::type_id_with_cvr;

  decltype(std::declval<Derived2<int>>().t()) a{};
  decltype(std::declval<Derived1<int>>().t()) b{};

  std::cout << "type(a) = " << type_id_with_cvr<decltype(a)>().pretty_name()
            << std::endl;
  std::cout << "type(b) = " << type_id_with_cvr<decltype(b)>().pretty_name()
            << std::endl;

  std::cout << a << ',' << b << '\n';
}

} // namespace ns_declval_test

TEST(declval_test, test1) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  ns_declval_test::declval_basic_test1();

  oss << "decltype(std::declval<NonDefault>()) = int\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

TEST(declval_test, test2) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  ns_declval_test::declval_basic_test2();

  oss << "type(a) = int\n"
         "type(b) = int\n"
         "0,0\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}