#include <algorithm>
#include <functional>
#include <gtest/gtest.h>
#include <random>

/// 在写c++程序时，需要产生n个连续的序列,如 1-10, 3-20等等. 此时可以使用
/// generate/generate_n 这两个方法来实现。

/// \code
/// template <typename ForwardIt, typename Generator>
/// void generate(ForwardIt first, ForwardIt last, Generator gen);
/// \endcode
/// This is equivalent to
/// \code
template <class ForwardIt, class Generator>
constexpr void ss_generate(ForwardIt first, ForwardIt last, Generator g) {
  while (first != last)
    *first++ = g();
}

template <typename T> class GenInitVal {
private:
  T init_val_;

public:
  explicit GenInitVal(T init_val = T{}) : init_val_(init_val) {}
  T operator()() { return ++init_val_; };
};

TEST(generate, generate_basic_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::vector<int> pool(10, 0);

  std::generate(pool.begin(), pool.end(), GenInitVal(1));

  for (const auto &val : pool)
    std::cout << val << ' ';
  std::cout << '\n';
  oss << "2 3 4 5 6 7 8 9 10 11 \n";

  ss_generate(pool.begin(), pool.end(), GenInitVal(1));

  for (const auto &val : pool)
    std::cout << val << ' ';
  std::cout << '\n';
  oss << "2 3 4 5 6 7 8 9 10 11 \n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

template <typename OutputIt, typename Size, typename Generator>
constexpr OutputIt ss_generate_n(OutputIt first, Size count, Generator g) {
  for (Size i = 0; i < count; ++i)
    *first++ = g();

  return first;
}

TEST(generate, generate_n_basic_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::mt19937 rng; // default constructed, seeded with fixed seed.
  std::generate_n(
      std::ostream_iterator<std::mt19937::result_type>(std::cout, " "), 5,
      std::ref(rng));
  std::cout << '\n';
  oss << "3499211612 581869302 3890346734 3586334585 545404204 \n";

  std::vector<int> pool;
  std::generate_n(std::ostream_iterator<int>(std::cout, " "), 9, GenInitVal(3));
  std::cout << '\n';
  oss << "4 5 6 7 8 9 10 11 12 \n";

  ss_generate_n(std::ostream_iterator<int>(std::cout, " "), 9, GenInitVal(3));
  oss << "4 5 6 7 8 9 10 11 12 ";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}
