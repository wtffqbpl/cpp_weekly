#include <gtest/gtest.h>
#include <iomanip>
#include <numeric>

TEST(std_copy_test, basic_test0) {
  std::vector<int> seq = {3, 4, 7, 9, 2, 5, 7, 8};
  std::vector<int> v;

  v.resize(seq.size());
  std::copy(seq.begin(), seq.end(), v.begin());

  std::stringstream oss;
  testing::internal::CaptureStdout();

  for (auto val : v)
    std::cout << val << ' ';
  std::cout << '\n';
  oss << "3 4 7 9 2 5 7 8 \n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "expected output:\n"
            << oss.str() << "actual output:\n"
            << act_output << std::endl;
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

TEST(std_copy_test, basic_test) {
  int buf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::copy(buf, buf + 10, std::ostream_iterator<int>(std::cout, " "));
  oss << "0 1 2 3 4 5 6 7 8 9 ";

  std::string act_output = testing::internal::GetCapturedStdout();
#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << "Actual output:\n"
            << act_output << std::endl;
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

TEST(std_copy_test, basic_test2) {
  std::stringstream oss;
  oss << "RECV ";
  int Buf[] = {0x68, 0x14, 0x02, 0x00, 0x00, 0x00, 0x67, 0x01,
               0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12,
               0x34, 0x0C, 0x13, 0x01, 0x07, 0x10};

  std::copy(Buf, Buf + sizeof(Buf) / sizeof(int),
            std::ostream_iterator<int>(oss, " "));

  testing::internal::CaptureStdout();
  std::cout << oss.str();

  std::string act_output = testing::internal::GetCapturedStdout();
  std::cout << std::endl;

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << "Actual output:\n"
            << act_output << std::endl;
#endif

  EXPECT_TRUE(act_output == oss.str());
}

TEST(std_copy_test, basic_test3) {
  std::stringstream oss;

  std::vector<int> Vec(10);
  std::iota(Vec.begin(), Vec.end(), 0);

  testing::internal::CaptureStdout();
  std::copy(Vec.begin(), Vec.end(), std::ostream_iterator<int>(std::cout, " "));
  std::string act_output = testing::internal::GetCapturedStdout();

  oss << "0 1 2 3 4 5 6 7 8 9 ";

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}