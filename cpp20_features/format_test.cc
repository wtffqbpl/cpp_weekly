#if __cplusplus >= 202002L

#ifdef __APPLE__

#include "internal_check_conds.h"

#include <format>
#include <gtest/gtest.h>
#include <string>

namespace {

void print2(const auto &arg1, const auto &arg2) {
  std::cout << std::format("args: {} and {}\n", arg1, arg2);
}

void basic_example() {
  std::string str{"hello"};

  // std::format
  // It allows them to combine a formatting string to be filled with values of
  // passed arguments according to the formatting specified inside a pair of
  // braces.
  std::cout << std::format("String '{}' has {} chars\n", str, str.size());

  // An optional integral value right after the opening braces specifies the
  // index of the argument so that you can process the arguments in different
  // order or use them multiple times.
  std::cout << std::format("String '{1}' has {0} chars\n", str, str.size());

  print2(str, str.size());
}

void format_alignment_example() {

  // Inside the placeholder for formatting after a colon, you can specify
  // details of the formatting of the argument.
  // Note that different types have different default alignments.
  std::cout << std::format("{:7}\n", 42);   // yields "     42"
  std::cout << std::format("{:7}\n", 42.0); // yields "     42"
  std::cout << std::format("{:7}\n", 'x');  // yields "x      "
  std::cout << std::format("{:7}\n", true); // yields "true   "

  // You can also specify the alignment explicitly:
  //  - < for left,
  //  - ^ for center
  //  - > for right
  // And it allows to specify a fill character:
  std::cout << std::format("{:*<7}\n", 42); // yields "42*****"
  std::cout << std::format("{:*>7}\n", 42); // yields "*****42"
  std::cout << std::format("{:*^7}\n", 42); // yields "**42***"

  // Specify precision (for limit strings to a certain size), fill characters,
  // or a positive sign:
  std::cout << std::format("{:7.2f} Euro\n", 42.0); // yields "  42.00 Euro"
  std::cout << std::format("{:7.4}\n", "corner");   // yields "corn   "

  // By using the position of the argument, we can print a value in multiple
  // forms.
  std::cout << std::format("'{0}' has value 0x{0:02x} {0:+4d} {0:03o}\n", '?');
  std::cout << std::format("'{0}' has value 0x{0:02x} {0:+4d} {0:03o}\n", 'y');
}

} // namespace

TEST(format_test, basic_test) {
  std::stringstream oss;

  testing::internal::CaptureStdout();
  basic_example();

  auto act_output = testing::internal::GetCapturedStdout();

  oss << "String 'hello' has 5 chars\n"
         "String '5' has hello chars\n"
         "args: hello and 5\n";

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

TEST(format_test, formatting_alignment_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  format_alignment_example();

  oss << "     42\n"
         "     42\n"
         "x      \n"
         "true   \n"
         "42*****\n"
         "*****42\n"
         "**42***\n"
         "  42.00 Euro\n"
         "corn   \n"
         "'?' has value 0x3f  +63 077\n"
         "'y' has value 0x79 +121 171\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

#endif // __APPLE__

#endif // __cplusplus >= 202002L
