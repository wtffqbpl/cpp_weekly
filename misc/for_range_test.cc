#include "internal_check_conds.h"
#include <gtest/gtest.h>
#include <map>

namespace for_range_test {

// The possible implementation of for-range loop
// {
//   auto && __range = range_expression;
//   for (auto __begin = begin_expr, __end = end_expr; __begin != __end;
//   ++__begin)
//   {
//     range_declaration = *__begin;
//     loop_statement;
//   }
// }

void print(std::map<int, std::string>::const_reference e) {
  std::cout << "key=" << e.first << ", value=" << e.second << std::endl;
}

void basic_test() {
  std::map<int, std::string> index_map{{1, "hello"}, {2, "world"}, {3, "!"}};

  std::for_each(index_map.begin(), index_map.end(), print);
}

class IntIter {
public:
  IntIter(int *p) : p_(p) {}
  bool operator!=(const IntIter &other) { return (p_ != other.p_); }

  const IntIter &operator++() {
    p_++;
    return *this;
  }

  int operator*() const { return *p_; }

private:
  int *p_;
};

template <unsigned int fix_size> class FixIntVector {
public:
  FixIntVector(std::initializer_list<int> init_list) {
    int *cur = data_;
    for (auto e : init_list) {
      *cur = e;
      ++cur;
    }
  }

  IntIter begin() { return IntIter{data_}; }
  IntIter end() { return IntIter{data_ + fix_size}; }

private:
  int data_[fix_size]{0};
};

void self_iter_for_range_test() {
  FixIntVector<10> fix_int_vector{1, 3, 5, 7, 9};

  for (auto e : fix_int_vector)
    std::cout << e << std::endl;
}

} // namespace for_range_test

TEST(for_range_test, base_test) { for_range_test::basic_test(); }

TEST(for_range_test, self_iter_for_range_test) {
  for_range_test::self_iter_for_range_test();
}