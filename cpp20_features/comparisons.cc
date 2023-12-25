#include <gtest/gtest.h>

#include <algorithm>
#include <compare>

#include "internal_check_conds.h"

namespace {

// When the compiler finds no matching declaration for an expression `a != b`,
// the compiler rewrites the expressions and looks for `!(a == b) instead. If
// that does not work, the compiler also tries to change the order of the
// operands, so it also tries `!(b == a)`. Therefore, for a of TypeA and b of
// TypeB, the compiler will be able to compile
//   a != b
// It can do this if there is
//  * A free-standing operator !=(TypeA, TypeB)
//  * A free-standing operator ==(TypeA, TypeB)
//  * A free-standing operator ==(TypeB, TypeA)
//  * A member function TypeA::operator!=(TypeB)
//  * A member function TypeA::operator==(TypeB)
//  * A member function TypeB::operator==(TypeA)
// Directly calling a defined operator != is preferred (but the order of the
// type has to fit). Changing the order of the operands has the lowest priority.
// Having both a free-standing and a member function is an ambiguity error.
//
// Note that thanks to rewriting, implicit type conversions for the first
// operand are also possible when rewriting converts the operand so that it
// becomes the parameter of the defined member function.

struct NullTerm {
  bool operator==(auto pos) const {
    return *pos == '\0'; // end is where iterator points to '\0'.
  }
};

void test_comparison_rewriting() {
  const char *raw_str = "hello world";

  // iterate over the range of the beginning of raw_str and its ends:
  for (auto pos = raw_str; pos != NullTerm{}; ++pos) {
    std::cout << ' ' << *pos;
  }
  std::cout << '\n';

  // call range algorithm with iterator and sentinel:
  std::ranges::for_each(raw_str, NullTerm{},
                        [](char c) { std::cout << ' ' << c; });

  std::cout << '\n';
}

class Value {
private:
  long id;

public:
  constexpr explicit Value(long i) noexcept : id{i} {}

  // for equality operators:
  bool operator==(const Value &rhs) const {
    return id == rhs.id; // defines equality (== and !=)
  }

  // Operator <=> is a new binary operator. It is defined for all fundamental
  // data types for which the relational operators are defined. As usual, it
  // can be user-defined as operator<=>().
  //
  // The new operator <=> does not return a Boolean value. Instead, it acts
  // similarly to three-way-comparisons yielding a negative value to signal
  // less, a positive value to signal greater, and 0 to signal equal or
  // equivalent.

  // for relational operators:
  auto operator<=>(const Value &rhs) const {
    return id <=> rhs.id; // defines ordering (<, <=, >, and >=)
  }
};

} // namespace

TEST(copmarison_test, test_rewriting) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  test_comparison_rewriting();

  auto act_output = testing::internal::GetCapturedStdout();

  oss << " h e l l o   w o r l d\n"
         " h e l l o   w o r l d\n";

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

TEST(comparison_test, three_way_comparison_test) {
  auto x = 3 <=> 4;

  EXPECT_TRUE(x < 0);
}