#include "internal_check_conds.h"
#include <gtest/gtest.h>

// The basic recursion version.
namespace {

template <typename... Ts> void print(Ts... args) {
  (std::cout << ... << args) << std::endl;
}

template <size_t N, size_t I> struct ForEach {

  static void item() {
    std::cout << I << std::endl;

    // recursive upwards
    if constexpr (I + 1 < N)
      ForEach<N, I + 1>::item();
  }
};

template <size_t N, size_t I = 0, size_t Step = 1, typename... Ts>
void for_loop_compile_time(Ts... args) {
  print(args...);

  if constexpr (I + Step < N)
    for_loop_compile_time<N, I + Step, Step, Ts...>(args...);
}

} // namespace

TEST(for_loop_in_compile_time, basic_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  ForEach<10, 0>::item();
  oss << "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n";

  auto act_output = testing::internal::GetCapturedStdout();
  debug_msg(oss, act_output);
  EXPECT_TRUE(oss.str() == act_output);
}

TEST(for_loop_in_compile_time, function_loop_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  for_loop_compile_time<10>(1, 2, 3, 4);
  oss << "1234\n1234\n1234\n1234\n1234\n1234\n1234\n1234\n1234\n1234\n";

  auto act_output = testing::internal::GetCapturedStdout();
  debug_msg(oss, act_output);
  EXPECT_TRUE(oss.str() == act_output);
}

// Static Table Generation

#include <array>

namespace {
constexpr int TABLE_SIZE = 10;

/**
 * Variadic template for a recursive helper struct.
 */
template <int INDEX = 0, int... D>
struct Helper : Helper<INDEX + 1, D..., INDEX * INDEX> {};

/*
 * Specialization of the template to end the recursion when the table
 * size reaches TABLE_SIZE.
 */
template <int... D> // end of the recursion.
struct Helper<TABLE_SIZE, D...> {
  static constexpr std::array<int, TABLE_SIZE> table = {D...};
};
// The idea behind this is that the struct Helper recursively inherits from
// a struct with one more template argument (in this example calculated as
// INDEX * INDEX) until the specialization of the template ends the recursion
// at a size of 10 elements.

constexpr std::array<int, TABLE_SIZE> table = Helper<>::table;

enum {
  FOUR = table[2] // compile time use.
};

// the expansion is as follows:
// template<> struct Helper<0, <>> : Helper<0 + 1, 0 * 0> {};
// template<> struct Helper<1, <0>> : Helper<1 + 1, 0, 1 * 1> {};
// template<> struct Helper<2, <0, 1>> : Helper<2 + 1, 0, 1, 2 * 2> {};
// template<> struct Helper<3, <0, 1, 4>> : Helper<3 + 1, 0, 1, 4, 3 * 3> {};
// ...
// template<> struct Helper<10, <0, 1, 4, 9, 16, 25, 36, 49, 64, 81>> {
//   static constexpr std::array<int, TABLE_SIZE> table = {0, 1, 4, 9, 16,
//                                                         25, 36, 49, 64, 81};
// };

// Since C++17, this can be more readably written as follows.

// This is a variable template, and receive return value from an immediate
// invoked lambda expression.
constexpr std::array<int, TABLE_SIZE> table_c17 = [] {
  // OR: constexpr auto table
  std::array<int, TABLE_SIZE> A = {};
  for (unsigned i = 0; i < TABLE_SIZE; ++i)
    A[i] = static_cast<int>(i * i);
  return A;
}();

} // namespace

TEST(static_table_generation, test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  for (int i = 0; i < TABLE_SIZE; ++i)
    std::cout << table[i] << std::endl; // run time use.
  std::cout << "FOUR: " << FOUR << std::endl;
  oss << "0\n1\n4\n9\n16\n25\n36\n49\n64\n81\nFOUR: 4\n";

  auto act_output = testing::internal::GetCapturedStdout();
  debug_msg(oss, act_output);
  EXPECT_TRUE(oss.str() == act_output);
}

TEST(static_table_generation, cpp17_version_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  for (unsigned i = 0; i < TABLE_SIZE; ++i)
    std::cout << table_c17[i] << std::endl; // run time use.
  oss << "0\n1\n4\n9\n16\n25\n36\n49\n64\n81\n";

  auto act_output = testing::internal::GetCapturedStdout();
  debug_msg(oss, act_output);
  EXPECT_TRUE(oss.str() == act_output);
}