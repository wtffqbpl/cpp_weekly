#include "internal_check_conds.h"
#include <gtest/gtest.h>

namespace {

// The definition of `placeholdes` is is follows:

/// \code
/// namespace placeholders {
/// extern /* unspecified */ _1;
/// extern /* unspecified */ _2;
/// extern /* unspecified */ _3;
/// ...
/// }
/// \endcode

void goodbye(const std::string &s) { std::cout << "Goodbye " << s << '\n'; }

class Object {
public:
  void hello(const std::string &s) { std::cout << "Hello " << s << '\n'; }
};

void test_placeholder() {
  typedef std::function<void(const std::string &)> ExampleFunction;
  Object instance{};
  std::string str("World");

  ExampleFunction f =
      std::bind(&Object::hello, &instance, std::placeholders::_1);
  // equivalent to instance.hello(str)
  f(str);
  f = std::bind(&goodbye, std::placeholders::_1);

  // equivalent to goodbye(str)
  f(str);
}

} // namespace

TEST(placeholder_test, baseic_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  test_placeholder();
  oss << "Hello World\n"
         "Goodbye World\n";

  auto act_output = testing::internal::GetCapturedStdout();
  debug_msg(oss, act_output);
  EXPECT_TRUE(oss.str() == act_output);
}