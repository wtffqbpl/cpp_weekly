#include <gtest/gtest.h>

template <typename T> void print(T arg) { std::cout << arg << '\n'; }

template <typename T, typename... Types> void print(T firstArg, Types... args) {
  print(firstArg); // call print() for the first argument.
  print(args...);  // call print() for remaining arguments.
}

TEST(chap4_variadic_templates, print_test) {
  std::stringstream oss;

  // Start to capture stdout.
  testing::internal::CaptureStdout();

  print(7.5, "Hello", "world", 18.0);
  oss << "7.5\nHello\nworld\n18\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << "Actual output:\n"
            << act_output << std::endl;
#endif

  EXPECT_TRUE(oss.str() == act_output);
}