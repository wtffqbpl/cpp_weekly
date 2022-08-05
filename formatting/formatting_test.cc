#include <gtest/gtest.h>
#include <iostream>
#include <iomanip>

TEST(Formatting, floatFormatTest){
  double pi = 3.141592653;
  std::string expected_str = "";

  testing::internal::CaptureStdout();

  // pi is 3.14159
  std::cout << "pi is " << pi << "\n";
  expected_str += "pi is 3.14159\n";

  // pi is 3.141592653
  std::cout << "pi is " << std::setprecision(16) << pi << std::endl;
  expected_str += "pi is 3.141592653\n";

  // pi is                    3.141592653
  std::cout << "pi is " << std::setw(30) << pi << std::endl;
  expected_str += "pi is                    3.141592653\n";

// pi is 3.141592653-------------------
  std::cout << "pi is " << std::setfill('-') << std::left << std::setw(30) << pi
            << std::endl;
  expected_str += "pi is 3.141592653-------------------\n";

  // pi is +3.1415926530000000e+00
  std::cout.setf(std::ios_base::showpos);
  std::cout << "pi is " << std::scientific << pi << "\n";
  expected_str += "pi is +3.1415926530000000e+00\n";

// 63 octal is 77.
  std::cout << "63 octal is " << std::oct << 63 << ".\n";
  expected_str += "63 octal is 77.\n";

  // 63 hexadecimal is 3f.
  std::cout << "63 hexadecimal is " << std::hex << 63 << ".\n";
  expected_str += "63 hexadecimal is 3f.\n";

// 63 decimal is +63.
  std::cout << "63 decimal is " << std::dec << 63 << ".\n";
  expected_str += "63 decimal is +63.\n";

  std::string act_output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(expected_str == act_output);
}

TEST(Formatting, intFormatingTest1) {
  std::string expected_str = "";

  // Capture stdout using gtest api.
  testing::internal::CaptureStdout();

  std::cout << 127 << ' ' << 255 << std::endl;
  expected_str += "127 255\n";

  std::cout << std::hex << 127 << ' ' << 255 << std::endl;
  expected_str += "7f ff\n";

  std::cout.setf(std::ios::showbase);
  std::cout << 127 << ' ' << 255 << std::endl;
  expected_str += "0x7f 0xff\n";

  std::cout.setf(std::ios::uppercase);
  std::cout << 127 << ' ' << 255 << std::endl;
  expected_str += "0X7F 0XFF\n";

  std::string act_output = testing::internal::GetCapturedStdout();
  std::cout << act_output << std::endl;
  EXPECT_TRUE(act_output == expected_str);
}