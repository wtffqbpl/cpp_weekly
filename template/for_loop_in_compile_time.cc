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
  oss << "1234\n"
         "1234\n"
         "1234\n"
         "1234\n"
         "1234\n"
         "1234\n"
         "1234\n"
         "1234\n"
         "1234\n"
         "1234\n";

  auto act_output = testing::internal::GetCapturedStdout();
  debug_msg(oss, act_output);
  EXPECT_TRUE(oss.str() == act_output);
}