#include <gtest/gtest.h>

std::string get_value(const bool b) {
  std::string ret;

  if (b)
    ret = "Hello";
  else
    ret = "World";

  return ret;
}

std::string get_value_multiple_return(const bool b) {
  return b ? "Hello" : "World";
}

TEST(cpp_weekly_youtube, multiple_return_test) {
#ifndef NDEBUG
  std::cout << "get_value(true) = " << get_value(true) << '\n'
            << "get_value(false) = " << get_value(false) << '\n'
            << "get_value_multiple_return(true) = "
            << get_value_multiple_return(true) << '\n'
            << "get_value_multiple_return(false) = "
            << get_value_multiple_return(false) << '\n';
#endif

  EXPECT_EQ(get_value(true), "Hello");
  EXPECT_EQ(get_value(false), "World");
  EXPECT_EQ(get_value_multiple_return(true), "Hello");
  EXPECT_EQ(get_value_multiple_return(false), "World");
}