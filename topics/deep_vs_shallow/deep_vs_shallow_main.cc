#include "deep_vs_shallow.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <iterator>

TEST(deep_vs_shallow, basic_test) {
  std::ostringstream oss;
  testing::internal::CaptureStdout();

  // 1. shallow copy
  Shallow obj_a;
  for (int i = 0; i < 10; ++i)
    obj_a.add(i);

  auto *obj_b = new Shallow(obj_a);
  obj_b->getElemAt(0) = 100;

  std::copy(obj_a.begin(), obj_a.end(),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << '\n';
  oss << "100 1 2 3 4 5 6 7 8 9\n";

  // 2. deep copy.
  Deep obj_c;
  for (int i = 0; i < 10; ++i)
    obj_c.add(i);

  auto *obj_d = new Deep(obj_c);
  obj_d->getElemAt(0) = 100;

  std::copy(obj_c.begin(), obj_c.end(),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << '\n';
  oss << "0 1 2 3 4 5 6 7 8 9\n";

  delete obj_b;
  delete obj_d;

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}