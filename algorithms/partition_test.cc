#include <algorithm>
#include <gtest/gtest.h>

/// \code
/// template< class BidirIt, class UnaryPredicate >
/// BidirIt stable_partition( BidirIt first, BidirIt last, UnaryPredicate p )
/// \endcode
// Reorders the elements in the range [first, last) in such a way that all
// elements for which the predicate p returns true precede the elements for
// which predicate p returns false. Relative order of the elements is preserved.
// first, last:
// 指向序列中初始及最后元素的位置的双向迭代器(bidirectional iterator),
// 范围是[first, last) pred:
// 一元谓词就是返回bool类型的一元表达式，接收范围中的一个元素作为实参，返回bool类型的一个
// 值，将所有返回true的元素放在返回false元素的前面
TEST(partition_test, basic_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::vector<int> pool{0, 0, 3, -1, 2, 4, 5, 0, 7};
  std::stable_partition(pool.begin(), pool.end(), [](int n) { return n > 0; });

  for (const auto val : pool)
    std::cout << val << ' ';
  oss << "3 2 4 5 7 0 0 -1 0 ";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

// partition 的返回值：
// 返回指向第二组(即pred为false)的第一个元素的迭代器，如果第二组为空，则返回end();
TEST(partition_test, test2) {
  std::ostringstream oss;
  testing::internal::CaptureStdout();

  std::vector<int> demo_vector;

  // set some value
  for (int i = 1; i < 10; ++i)
    demo_vector.push_back(i);

  std::vector<int>::iterator iBound;
  iBound = std::stable_partition(demo_vector.begin(), demo_vector.end(),
                                 [](int val) { return (val % 2) == 1; });

  // print out the content.
  std::cout << "After reorder elements:";
  for (const int val : demo_vector)
    std::cout << val << ' ';
  std::cout << '\n';
  oss << "After reorder elements:1 3 5 7 9 2 4 6 8 \n";

  std::cout << "Old element:";
  for (auto it = demo_vector.begin(); it != iBound; ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';
  oss << "Old element: 1 3 5 7 9\n";

  std::cout << "Even elements:";
  for (auto it = iBound; it != demo_vector.end(); ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';
  oss << "Even elements: 2 4 6 8\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

TEST(partition_test, partition_stable_partition_test) {
  std::string input = "***b**a**c**d**";
  std::string str1{input};
  std::string str2{input};

  std::partition(str1.begin(), str1.end(),
                 [](const auto val) { return val == '*'; });
  std::stable_partition(str2.begin(), str2.end(),
                        [](const auto val) { return val == '*'; });

  std::stringstream oss;
  testing::internal::CaptureStdout();
  std::cout << "str1 = " << str1 << '\n';
  std::cout << "str2 = " << str2 << '\n';
  oss << "str1 = ***********cdab\n"
      << "str2 = ***********bacd\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}
