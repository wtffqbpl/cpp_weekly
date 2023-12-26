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

class Person {
private:
  std::string name;
  double value;

public:
  auto operator<=>(const Person &rhs) const
  -> std::common_comparison_category_t<decltype(name <=> rhs.name),
                                       decltype(value <=> rhs.value)> {
    using comp1_t = decltype(name <=> rhs.name);

    auto cmp1 = name <=> rhs.name; // return strong-ordering for std::string
    if (cmp1 != comp1_t::equal) return cmp1;
    return value <=> rhs.value;
  }
};

// If and only if an operator<=> member is defined as defaulted, then by
// definition a corresponding operator== member is also defined if no defaulted
// operator== is provided. All aspects (visibility, virtual, attributes,
// requirements, etc.) are adopted.
template <typename T> class Type {
public:
  [[nodiscard]] std::strong_ordering operator<=>(const Type &) const
    requires(!std::same_as<T, bool>)
  = default;
};
// is equivalent to the following:
template <typename T> class TypeNew {
public:
  [[nodiscard]] std::strong_ordering operator<=>(const TypeNew &) const
    requires(!std::same_as<T, bool>)
  = default;

  [[nodiscard]] bool operator==(const TypeNew &) const
    requires(!std::same_as<T, bool>)
  = default;
};

struct Coord {
  // This is enough to support all six comparison operators for objects.
  double x{};
  double y{};
  double z{};

  auto operator<=>(const Coord &) const = default;
};

void test_default_operators() {
  std::vector<Coord> coll{
      {0, 5, 5}, {5, 0, 0}, {3, 5, 5}, {3, 0, 0}, {3, 5, 7}};

  std::sort(coll.begin(), coll.end());
  for (const auto &elem : coll) {
    std::cout << elem.x << '/' << elem.y << '/' << elem.z << '\n';
  }
}

std::ostream &operator<<(std::ostream &os, std::strong_ordering val) {
  if (val == std::strong_ordering::less)
    return os << "less";
  if (val == std::strong_ordering::greater)
    return os << "greater";
  return os << "equal";
}

void lexico_three_way_test() {
  std::vector v1{0, 8, 15, 47, 11};
  std::vector v2{0, 15, 8};

  auto r1 =
      std::lexicographical_compare(v1.begin(), v1.end(), v2.begin(), v2.end());

  auto r2 = std::lexicographical_compare_three_way(v1.begin(), v1.end(),
                                                   v2.begin(), v2.end());

  std::cout << "r1: " << r1 << '\n';
  std::cout << "r2: " << r2 << '\n';
}

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

TEST(comparison_test, default_comparison_operators_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  test_default_operators();

  oss << "0/5/5\n"
         "3/0/0\n"
         "3/5/5\n"
         "3/5/7\n"
         "5/0/0\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

TEST(comparison_test, lexico_three_way_test) {
  std::stringstream oss;

  testing::internal::CaptureStdout();

  lexico_three_way_test();

  oss << "r1: 1\n"
         "r2: less\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}