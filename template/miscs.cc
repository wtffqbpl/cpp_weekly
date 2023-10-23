#include "internal_check_conds.h"
#include <gtest/gtest.h>
#include <type_traits>

namespace is_copy_assignable_test {

class AcpAble {};

class BcpAble {
public:
  BcpAble &operator=(const BcpAble &tmp_obj) { return *this; }
};

class CcpAble {
public:
  CcpAble &operator=(const CcpAble &tmp_obj) = delete;
};

void is_copy_assignable_test() {
  std::cout << "int: " << std::is_copy_assignable_v<int> << std::endl;
  std::cout << "AcpAble: " << std::is_copy_assignable_v<AcpAble> << std::endl;
  std::cout << "BcpAble: " << std::is_copy_assignable_v<BcpAble> << std::endl;
  std::cout << "CcpAble: " << std::is_copy_assignable_v<CcpAble> << std::endl;
}

} // namespace is_copy_assignable_test

TEST(is_copy_assignable_test, test1) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  is_copy_assignable_test::is_copy_assignable_test();

  oss << "int: 1\n"
         "AcpAble: 1\n"
         "BcpAble: 1\n"
         "CcpAble: 0\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}