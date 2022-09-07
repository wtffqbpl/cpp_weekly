#include <gtest/gtest.h>

std::string get_value(const bool b) {
  std::string ret;

  if (b)
    ret = "Hello";
  else
    ret = "World";

  return ret;
}

// 在实际写代码过程中，更加推荐这种多出口的写法，这种只会有一个构造函数的调用过程，
// 不会像上面那种写法，会出现每个分支都有一个构造函数的场景.
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