#include <gtest/gtest.h>
#include <typeinfo>

TEST(typeinfo_test, name_test) {
  bool a;
  short b;
  int c;
  unsigned d;

  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::cout << "bool a type is " << typeid(a).name() << std::endl;
  std::cout << "short b type is " << typeid(b).name() << std::endl;
  std::cout << "int c type is " << typeid(c).name() << std::endl;
  std::cout << "unsigned d type is " << typeid(d).name() << std::endl;
  // set expected output.
  oss << "bool a type is b\n"
      << "short b type is s\n"
      << "int c type is i\n"
      << "unsigned d type is j\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output: \n"
            << oss.str() << "Actual output:\n"
            << act_output << std::endl;
#endif

  EXPECT_TRUE(act_output == oss.str());
}