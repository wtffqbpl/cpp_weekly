#include <gtest/gtest.h>
#include <tuple>

#include "internal_check_conds.h"

namespace structured_binding {

constexpr std::tuple<int, int> return_multiple_values() {
  return std::make_tuple(11, 7);
}

void basic_test() {
  int x = 0, y = 0;
  std::tie(x, y) = return_multiple_values();

  std::cout << "x = " << x << " y = " << y << std::endl;
}

auto return_multiple_values_cpp_17() { return std::make_tuple(11, 7); }

void structured_binding_test() {
  auto [x, y] = return_multiple_values_cpp_17();

  std::cout << "x = " << x << " y = " << y << std::endl;
}

struct BindTest {
  int a = 42;
  std::string b = "hello structured binding";
};

void structured_binding_test_2() {
  BindTest bt;
  auto [x, y] = bt;

  // the pseudo code is:
  // BindTest bt;
  // const auto _anonymous = bt;
  // aliasname x = = _anonymous.a;
  // aliasname y = _anonymous.b;

  std::cout << "x = " << x << " y = " << y << std::endl;
}

void structured_binding_test_3() {
  std::vector<BindTest> bt{{11, "hello"}, {7, "c++"}, {42, "world"}};

  for (const auto &[x, y] : bt)
    std::cout << "x = " << x << " y = " << y << std::endl;
}

void structured_binding_test_4() {
  BindTest bt;
  const auto [x, y] = bt;

  std::cout << std::boolalpha;
  std::cout << "Is bt.a and x has the same address? "
            << ((std::uintptr_t)(&bt.a) == (std::uintptr_t)(&x)) << std::endl;

  std::cout << "Is bt.b and y has the same address? "
            << ((std::uintptr_t)(&bt.b) == (std::uintptr_t)(&y)) << std::endl;

  std::cout << "std::is_same_v<const int, decltype(x)>="
            << std::is_same_v<const int, decltype(x)> << std::endl;

  std::cout << "std::is_same_v<const std::string, decltype(y)>="
            << std::is_same_v<const std::string, decltype(y)> << std::endl;
}

} // namespace structured_binding

TEST(structured_binding, basic_test) {
  std::stringstream oss;

  testing::internal::CaptureStdout();

  structured_binding::basic_test();
  auto act_output = testing::internal::GetCapturedStdout();

  oss << "x = 11 y = 7\n";

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

TEST(structured_binding, new_feature_test) {
  testing::internal::CaptureStdout();
  structured_binding::structured_binding_test();
  auto act_output = testing::internal::GetCapturedStdout();

  std::stringstream oss;
  oss << "x = 11 y = 7\n";

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

TEST(structured_binding, new_feature_test2) {
  testing::internal::CaptureStdout();
  structured_binding::structured_binding_test_2();
  auto act_output = testing::internal::GetCapturedStdout();

  std::stringstream oss;
  oss << "x = 42 y = hello structured binding\n";

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

TEST(structured_binding, new_feature_test3) {
  testing::internal::CaptureStdout();
  structured_binding::structured_binding_test_3();
  auto act_output = testing::internal::GetCapturedStdout();

  std::stringstream oss;
  oss << "x = 11 y = hello\n"
      << "x = 7 y = c++\n"
      << "x = 42 y = world\n";

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

TEST(structured_binding, new_feature_test4) {
  testing::internal::CaptureStdout();
  structured_binding::structured_binding_test_4();
  auto act_output = testing::internal::GetCapturedStdout();

  std::stringstream oss;
  oss << "Is bt.a and x has the same address? false\n"
         "Is bt.b and y has the same address? false\n"
         "std::is_same_v<const int, decltype(x)>=true\n"
         "std::is_same_v<const std::string, decltype(y)>=true\n";

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

class BindBase3 {
public:
  int a = 42;
};

class BindTest3 : public BindBase3 {
public:
  double b = 11.7;
};

namespace std {
template <> struct tuple_size<BindTest3> {
  static constexpr std::size_t valeu = 2;
};

template <> struct tuple_element<0, BindTest3> { using type = int; };

template <> struct tuple_element<1, BindTest3> { using type = double; };

} // namespace std

template <std::size_t Idx> auto &get(BindTest3 &bt) = delete;

template <> auto &get<0>(BindTest3 &bt) { return bt.a; }

template <> auto &get<1>(BindTest3 &bt) { return bt.b; }

class BindTest3New : public BindBase3 {
public:
  double b = 11.7;

  template <std::size_t Idx> auto &get() = delete;
};

template <> auto &BindTest3New::get<0>() { return a; }
template <> auto &BindTest3New::get<1>() { return b; }

namespace std {
template <> struct tuple_size<BindTest3New> {
  static constexpr std::size_t value = 2;
};

template <> struct tuple_element<0, BindTest3New> { using type = int; };

template <> struct tuple_element<1, BindTest3New> { using type = double; };

} // namespace std

void structured_binding_test_5() {
  BindTest3New bt3;
  auto &[x, y] = bt3;
  x = 78;

  std::cout << bt3.a << std::endl;
}

TEST(structured_binding, test5) {
  testing::internal::CaptureStdout();
  structured_binding_test_5();
  auto act_output = testing::internal::GetCapturedStdout();

  std::stringstream oss;
  oss << "78\n";

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}
