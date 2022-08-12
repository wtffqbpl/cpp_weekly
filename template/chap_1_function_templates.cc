#include <gtest/gtest.h>
#include <iostream>

TEST(Chap_1_Function_Template, max1_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  int i = 42;
  std::cout << "max(7, i): " << std::max(7, i) << '\n';
  oss << "max(7, i): 42\n";

  double f1 = 3.4;
  double f2 = -6.7;
  std::cout << "max(f1, f2): " << std::max(f1, f2) << '\n';
  oss << "max(f1, f2): 3.4\n";

  std::string s1 = "mathematics";
  std::string s2 = "math";
  std::cout << "max(s1, s2): " << std::max(s1, s2) << '\n';
  oss << "max(s1, s2): mathematics\n";

  std::string act_output = testing::internal::GetCapturedStdout();
  std::cout << "Expected output:\n" << oss.str() << '\n'
            << "Actual output:\n" << act_output << '\n';

  EXPECT_TRUE(oss.str() == act_output);
}