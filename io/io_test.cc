#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <sstream>

void write_something(std::ostream &os) {
  os << "Hi stream, did you know that 3 * 3 = " << 3 * 3 << std::endl;
}

TEST(IOTest, coutTest) {
  std::string expected_str = "Hi stream, did you know that 3 * 3 = 9\n";

  testing::internal::CaptureStdout();
  write_something(std::cout);
  std::string act_output = testing::internal::GetCapturedStdout();

  EXPECT_TRUE(expected_str == act_output);
}

TEST(IOTest, fileTest) {
  std::string expected_str = "Hi stream, did you know that 3 * 3 = 9\n";
  std::ofstream myfile("example.txt");

  write_something(myfile);

  // read outputs.
  std::ifstream resfile("example.txt");
  std::stringstream oss;
  oss << resfile.rdbuf();
  std::string act_output = oss.str();

  EXPECT_TRUE(expected_str == act_output);
}

TEST(IOTest, streamTest) {
  std::stringstream mysstream;
  write_something(mysstream);

  std::string expected_str = "Hi stream, did you know that 3 * 3 = 9\n";

  std::string act_output = mysstream.str();

  std::cout << expected_str << std::endl;
  std::cout << act_output << std::endl;

EXPECT_TRUE(act_output == expected_str);
}