#include "internal_check_conds.h"
#include <boost/describe.hpp>
#include <boost/mp11.hpp>
#include <gtest/gtest.h>

namespace {
enum E {
  v1 = 1,
  v2 = 2,
  v3 = 3,
};

BOOST_DESCRIBE_ENUM(E, v1, v2, v3)

std::string reflection_test_impl() {
  using L1 = boost::describe::describe_enumerators<E>;
  boost::mp11::mp_for_each<L1>(
      [](auto D) { std::cout << D.name << ": " << D.value << std::endl; });

  std::stringstream oss;
  oss << "v1: 1\n"
      << "v2: 2\n"
      << "v3: 3\n";

  return oss.str();
}

} // namespace

TEST(reflection_test, basic_test) {
  testing::internal::CaptureStdout();

  // reflection test
  auto expected_output = reflection_test_impl();

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected:\n"
            << expected_output << "\nActual:\n"
            << act_output << std::endl;
#endif

  EXPECT_TRUE(expected_output == act_output);
}