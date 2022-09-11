#include <gtest/gtest.h>

TEST(small_string_optimization, sso_cow_test) {
  std::string a = "hello world";
  std::string b = a;
  bool res1 = a.c_str() == b.c_str();

  std::string c = b;
  c = "";
  bool res2 = a.c_str() == b.c_str();

  a = "";
  bool res3 = a.c_str() == b.c_str();

  EXPECT_FALSE(res1);
  EXPECT_FALSE(res2);
  EXPECT_FALSE(res3);
}