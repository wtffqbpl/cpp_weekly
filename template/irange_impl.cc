#include <gtest/gtest.h>

#include <algorithm>
#include <iterator>
#include <type_traits>

#include "internal_check_conds.h"

namespace detail {

// Returns false since we cannot have x < 0 if x is unsigned
template <typename T>
static inline constexpr bool is_negative(
    const T& /* x */,
    std::true_type /* is_unsigned */) {
return false;
}

// Returns true if a signed variable x < 0
template <typename T>
static inline constexpr bool is_negative(
    const T& x,
    std::false_type /* is_unsigned */) {
return x < T{0};
}

// Returns true if x < 0
// NOTE: Will fail on an unsigned custom type
//        For the most part it's possible to fix this if
//        the custom type has a constexpr constructor.
template <typename T>
inline constexpr bool is_negative(const T &x) { return is_negative(x, std::is_unsigned<T>{}); }

template <typename I,
          bool one_sided = false,
          std::enable_if_t<std::is_integral_v<I>, int> = 0>
struct integer_iterator {
  using iterator_category = std::input_iterator_tag;
  using value_type = I;
  using difference_type = std::ptrdiff_t;
  using pointer = I*;
  using reference = I&;

  explicit integer_iterator(I value) : value_{value} {}

  I operator*() const { return value_; }

  I const* operator->() const { return &value_; }

  integer_iterator &operator++() {
    ++value_;
    return *this;
  }

  integer_iterator operator++(int) {
    const auto copy = *this;
    ++*this;
    return copy;
  }

  bool operator==(const integer_iterator &other) const {
    if constexpr (one_sided) {
      // Range-for loops' end test is `begin != end`, not `begin < end`. To handle `irange(n)`
      // where n < 0 (which should be empty), we just make `begin != end` fail whenever `end` is
      // negative.
      return is_negative(other.value_) || value_ == other.value_;
    } else {
      return value_ == other.value_;
    }

    // Suppress "warning: missing return statement at end of non-void function"
    return false; // Horrible hack
  }

  bool operator!=(const integer_iterator &other) const { return !(*this == other); }

protected:
  I value_;
};

template <typename I,
          bool one_sided = false,
          std::enable_if_t<std::is_integral_v<I>, bool> = true>
struct integer_range {
public:
  integer_range(I begin, I end) : begin_{begin}, end_{end} {}
  using iterator = integer_iterator<I, one_sided>;

  iterator begin() const { return begin_; }
  iterator end() const { return end_; }

private:
  iterator begin_;
  iterator end_;
};

// Creates an integer range for the half-open interval [begin, end)
// If end <= begin, then the range is empty.
// The range has the type of the `end` integer; `begin` integer is cast to this type.

template <typename Integer1,
          typename Integer2,
          std::enable_if_t<std::is_integral_v<Integer1>, bool> = true,
          std::enable_if_t<std::is_integral_v<Integer2>, bool> = true>
integer_range<Integer2> irange(Integer1 begin, Integer2 end) {
  // If end <= begin then the range is empty; we can achieve this effect by choosing the larger
  // {begin, end} as the loop terminator
  return {
    static_cast<Integer2>(begin),
    std::max(static_cast<Integer2>(begin), end)};
}

// Creates an integer range for the half-open interval [0, end)
// If end <= begin, then the range is empty
template <typename Integer,
          std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
integer_range<Integer, true> irange(Integer end) {
  return {Integer{0}, end};
}

} // namespace detail

TEST(irange_test, test1) {
  testing::internal::CaptureStdout();
  std::stringstream oss;

  for (auto i : detail::irange(10)) {
    std::cout << i << ", ";
  }
  std::cout << '\n';

  auto act_output = testing::internal::GetCapturedStdout();

  oss << "0, 1, 2, 3, 4, 5, 6, 7, 8, 9, \n";

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}