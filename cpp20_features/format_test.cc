#include "internal_check_conds.h"

#include <format>
#include <gtest/gtest.h>
#include <string>

namespace {

void basic_example() {
  std::string str{"hello"};

  std::cout << std::format("String '{}' has {} chars\n", str, str.size());
  std::cout << std::format("String '{1}' has {0} chars\n", str, str.size());
}

} // namespace

TEST(format_test, basic_test) {
  std::stringstream oss;

  testing::internal::CaptureStdout();
  basic_example();

  auto act_output = testing::internal::GetCapturedStdout();

  oss << "String 'hello' has 5 chars\n"
         "String '5' has hello chars\n";

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}