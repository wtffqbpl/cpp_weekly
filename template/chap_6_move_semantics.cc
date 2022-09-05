#include <gtest/gtest.h>
#include <utility>

class X {};

void g(X &) { std::cout << "g() for variable\n"; }
void g(X const &) { std::cout << "g() for constant\n"; }
void g(X &&) { std::cout << "g() for movable object\n"; }

// let f() perfect forward argument val to g();
template <typename T> void f(T &&val) {
  g(std::forward<T>(val)); // call the right g() for any passed argument val.
}

TEST(chap_6_move_semantics, perfect_forward_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  X v;
  X const c;

  f(v);            // f() for variable calls f(X&) => calls g(X&)
  f(c);            // f() for constant calls f(X cont&) => calls g(X const&)
  f(X{});          // f() for temporary calls f(X&&) => calls g(X&&)
  f(std::move(v)); // f() for move-enable variable calls f(X&&) => calls g(X&&)

  oss << "g() for variable\n"
         "g() for constant\n"
         "g() for movable object\n"
         "g() for movable object\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}