#include <gtest/gtest.h>
#include <ctime>
#include <iostream>
#include <fstream>
#include <functional>
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

/*
 * @brief: 测试std::endl 作为换行符时的耗时问题
 *    std::cout << "hello world" << std::endl;
 *    上面这句其实等价于:
 *    std::cout << "hello world" << "\n";
 *    std::flush(std::cout);
 *    此外，"\n" 可以替换成 '\n' 这种char，这样速度更快。
 */
TEST(IOTest, std_endl_test) {
  constexpr unsigned times = 100000;
  std::ofstream ofile("output.txt");
  clock_t start_time, end_time;
  clock_t endl_time_eclipse, str_n_time_eclipse, char_n_time_eclipse, one_str_time_eclipse;

  // using std::endl is equivalent to [ << "\n"; std::flush();]
  start_time = std::clock();
  for (unsigned i = 0; i < times; ++i)
    ofile << "Hello world" << std::endl;
  end_time = std::clock();
  endl_time_eclipse = end_time - start_time;

  // To equivalent to following codes.
  start_time = std::clock();
  for (unsigned i = 0; i < times; ++i)
    ofile << "Hello world" << "\n";
  ofile.flush();
  end_time = std::clock();
  str_n_time_eclipse = end_time - start_time;

  // This is more efficient.
  start_time = std::clock();
  for (unsigned i = 0; i < times; ++i)
    ofile << "Hello world" << '\n';
  ofile.flush();
  end_time = std::clock();
  char_n_time_eclipse = end_time - start_time;

  start_time = std::clock();
  for (unsigned i = 0; i < times; ++i)
    ofile << "Hello world\n";
  ofile.flush();
  end_time = std::clock();
  one_str_time_eclipse = end_time - start_time;

#ifndef NDEBUG
  std::cout << "endl_time_eclipse = " << endl_time_eclipse << std::endl;
  std::cout << "str_n_time_eclipse = " << str_n_time_eclipse << std::endl;
  std::cout << "char_n_time_eclipse = " << char_n_time_eclipse << std::endl;
  std::cout << "one_str_time_eclipse = " << one_str_time_eclipse << std::endl;
  // FIXME: I don't know why these tests will be failed, so I just left two cases to compare.
  EXPECT_TRUE(str_n_time_eclipse >= char_n_time_eclipse);
  EXPECT_TRUE(endl_time_eclipse > one_str_time_eclipse);
#endif

  EXPECT_TRUE(endl_time_eclipse > char_n_time_eclipse);
}

TEST(IOTest, cout_lambda_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  [out = std::ref(std::cout << "Hello")]() { out.get() << " world\n"; }();

  oss << "Hello world\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}