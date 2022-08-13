#include <gtest/gtest.h>
#include <optional>


// optional can be used as the return type of a factory taht may fail.
std::optional<std::string> create(bool b) {
  if (b)
    return "Godzilla";
  return {};
}

// std::nullopt can be used to create any (empty) std::optional
auto create2(bool b) {
  return b ? std::optional<std::string>{"Godzilla"} : std::nullopt;
}

// std::reference_wrapper may be used to return a reference.
auto create_ref(bool b) {
  static std::string value = "Godzilla";
  return b ? std::optional<std::reference_wrapper<std::string>>{value}
           : std::nullopt;
}

TEST(std_optional_test, basic_test1) {
  std::stringstream oss;

  testing::internal::CaptureStdout();

  std::cout << "create(false) returned "
            << create(false).value_or("empty") << '\n';
  oss << "create(false) returned empty\n";

  // optional-returning factory functions are usable as conditions of while and if
  if (auto str = create2(true)) {
    std::cout << "create2(true) returned " << *str << '\n';
  }
  oss << "create2(true) returned Godzilla\n";

if (auto str = create_ref(true)) {
    // using get() to access the reference_wrapper's value
    std::cout << "create_ref(true) returned " << str->get() << '\n';
    str->get() = "Mothra";
    std::cout << "modifying it changed it to " << str->get() << '\n';
  }
  oss << "create_ref(true) returned Godzilla\n";
  oss << "modifying it changed it to Mothra\n";

  // get stdout.
  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output: " << oss.str()
            << "Actual output: " << act_output << std::endl;
#endif

  EXPECT_TRUE(act_output == oss.str());
}