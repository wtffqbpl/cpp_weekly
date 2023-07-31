#include "internal_check_conds.h"
#include <gtest/gtest.h>

namespace tuple_impl {

template <typename... Types> class Tuple;

// recursive case:
template <typename Head, typename... Tail> class Tuple<Head, Tail...> {
private:
  Head head;
  Tuple<Tail...> tail;

public:
  // constructors
  Tuple() = default;

  Tuple(Head const &head, Tuple<Tail...> const &tail)
      : head(head), tail(tail) {}

  Head &getHead() { return head; }
  Head const &getHead() const { return head; }
  Tuple<Tail...> &getTail() { return tail; }
  Tuple<Tail...> const &getTail() const { return tail; }
};

// basis case:
template <> class Tuple<> {
  // no storage required
};

// GET FUNCTION

// recursive case:
template <unsigned N> struct TupleGet {
  template <typename Head, typename... Tail>
  static auto apply(Tuple<Head, Tail...> const &t) {
    return TupleGet<N - 1>::apply(t.getTail());
  }
};

// basis case:
template <> struct TupleGet<0> {
  template <typename Head, typename... Tail>
  static Head const &apply(Tuple<Head, Tail...> const &t) {
    return t.getHead();
  }
};

template <unsigned N, typename... Types> auto get(Tuple<Types...> const &t) {
  return TupleGet<N>::apply(t);
}

} // namespace tuple_impl

TEST(tuple_test, basic_test) { std::cout << "Hello world\n"; }