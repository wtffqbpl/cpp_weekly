#include "internal_check_conds.h"
#include <array>
#include <gtest/gtest.h>
#include <iostream>
#include <tuple>
#include <utility>

namespace integer_sequence_test {

// Defined in header <utility>  since C++14
/// \code
/// template <typename T, T... Ints>
/// class integer_sequence;
/// \endcode

// The class template std::integer_sequence represents a compile-time sequence
// of integers. When used as an argument to a function template, the parameter
// pack Ints can be deduced and used in pack expansion.

// Member functions:
/// size [public static] returns the number of elements in Ints.

// The helper templates:
// * std::index_sequence
/// \code
/// template <std::size_t... Ints>
/// using index_sequence = std::integer_sequence<std::size_t, Ints...>;
/// \endcode

// * make_integer_sequence
// template <typename T, T N>
// using make_integer_sequence = std::integer_sequence<T, N>;

// * make_index_sequence
// template <std::size_t N>
// using make_index_sequence = std::make_integer_sequence<std::size_t, N>;

// * index_sequence_for
//  std::index_sequence_for is defined to convert any type parameter pack into
//  an index sequence of the same length.
// template <typename... T>
// using index_sequence_for = std::make_index_sequence<sizeof...(T)>;

// debugging aid
template <typename T, T... ints>
void print_sequence(std::integer_sequence<T, ints...> int_seq) {
  std::cout << "The sequence of size " << int_seq.size() << ": ";
  ((std::cout << ints << ' '), ...);
  std::cout << '\n';
}

template <typename Array, std::size_t... I>
auto a2t_impl(const Array &a, std::index_sequence<I...>) {
  return std::make_tuple(a[I]...);
}

template <typename T, std::size_t N,
          typename Indices = std::make_index_sequence<N>>
auto a2t(const std::array<T, N> &a) {
  return a2t_impl(a, Indices{});
}

// pretty-print a tuple
template <typename Ch, typename Tr, typename Tuple, std::size_t... Is>
void print_tuple_impl(std::basic_ostream<Ch, Tr> &os, const Tuple &t,
                      std::index_sequence<Is...>) {
  ((os << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
}

template <typename Ch, typename Tr, typename... Args>
auto &operator<<(std::basic_ostream<Ch, Tr> &os, const std::tuple<Args...> &t) {
  os << "(";
  print_tuple_impl(os, t, std::index_sequence_for<Args...>{});
  return os << ")";
}

void test_integer_sequence() {
  print_sequence(std::integer_sequence<unsigned, 9, 2, 5, 1, 9, 1, 6>{});
  print_sequence(std::make_integer_sequence<int, 20>{});
  print_sequence(std::make_index_sequence<10>{});
  print_sequence(std::index_sequence_for<float, std::iostream, char>{});

  std::array<int, 4> array = {1, 2, 3, 4};

  // convert an array into a tuple.
  auto tuple = a2t(array);
  static_assert(std::is_same_v<decltype(tuple), std::tuple<int, int, int, int>>,
                "");

  // print it to cout
  std::cout << "The tuple: " << tuple << '\n';
}

} // namespace integer_sequence_test

TEST(integer_sequence_test, basic_test) {
  integer_sequence_test::test_integer_sequence();
}
