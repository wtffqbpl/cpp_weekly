#include "internal_check_conds.h"
#include <gtest/gtest.h>
#include <memory>

namespace {

class Widget {
private:
  std::string name_;

public:
  explicit Widget(std::string name) : name_(std::move(name)) {}

  friend std::ostream &operator<<(std::ostream &os, const Widget &w) {
    os << w.name_ << '\n';
    return os;
  }
};

// init capture --- generalized lambda capture --- since c++14
void test_init_capture(const std::string &name) {
  auto pw_outer = std::make_unique<Widget>(name);

  // init capture
  [pw = std::move(pw_outer)] { std::cout << *pw.get(); }();
  [pw = std::make_unique<Widget>(name)] { std::cout << *pw.get(); }();
}

} // namespace

TEST(lambda_expression, init_capture_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  test_init_capture("Name");
  oss << "Name\n";
  oss << "Name\n";

  auto act_output = testing::internal::GetCapturedStdout();
  debug_msg(oss, act_output);

  EXPECT_TRUE(oss.str() == act_output);
}