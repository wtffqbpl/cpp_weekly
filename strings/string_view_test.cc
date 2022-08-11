#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <string_view>

TEST(string_view_test, basics) {
  std::stringstream oss;

  testing::internal::CaptureStdout();

  const char *cstr = "string_view test";
  std::string_view sv1(cstr);
  std::string_view sv2(cstr, 4);
  std::cout << "sv1: " << sv1 << ", sv2: " << sv2 << std::endl;
  oss << "sv1: string_view test, sv2: stri" << std::endl;

  std::string str = "string_view test";
  std::string_view sv3{str.data()};
  std::string_view sv4{str.data(), 4};
  std::cout << "sv3: " << sv3 << ", sv4: " << sv4 << std::endl;
  oss << "sv3: string_view test, sv4: stri" << std::endl;

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Actual output: " << std::endl;
  std::cout << act_output << std::endl;
  std::cout << "Expected output: " << std::endl;
  std::cout << oss.str() << std::endl;
#endif

  EXPECT_TRUE(act_output == oss.str());
}