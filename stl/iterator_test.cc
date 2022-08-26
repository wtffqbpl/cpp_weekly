#include <gtest/gtest.h>

// implementation via tag dispatch, available in c++98 with constexpr removed.
namespace advance_v1 {
template <typename Iterator>
constexpr // required since c++17
    void
    do_advance(Iterator &It,
               typename std::iterator_traits<Iterator>::difference_type N,
               std::input_iterator_tag) {
  for (; N > 0; --N) ++It;
}

template <typename Iterator>
constexpr // required since c++17
    void
    do_advance(Iterator &It,
               typename std::iterator_traits<Iterator>::difference_type N,
               std::bidirectional_iterator_tag) {
  if (N >= 0)
    for (; N > 0; --N) ++It;
  else
    for (; N < 0; ++N) --It;
}

template <typename Iterator>
constexpr // required since c++17
void do_advance(Iterator &It,
               typename std::iterator_traits<Iterator>::difference_type N,
               std::random_access_iterator_tag) {
  It += N;
}

template <typename Iterator, typename Distance>
constexpr // required since c++17
void adv_v1(Iterator &It, Distance N) {
  do_advance(It,
             typename std::iterator_traits<Iterator>::difference_type(N),
             typename std::iterator_traits<Iterator>::iterator_category());
}

} // end of namespace advance_v1

namespace advance_v2 {
// implementation via constexpr if, available in c++17
template <typename Iterator, typename Distance>
constexpr void adv_v2(Iterator &It, Distance N) {
  using catetory = typename std::iterator_traits<Iterator>::iterator_category;
  static_assert(std::is_base_of_v<std::input_iterator_tag, catetory>);

  auto dist = typename std::iterator_traits<Iterator>::difference_type(N);
  if constexpr (std::is_base_of_v<std::random_access_iterator_tag, catetory>)
    It += dist;
  else  {
    for (; dist > 0; --dist) ++It;
    if constexpr (std::is_base_of_v<std::bidirectional_iterator_tag, catetory>)
      for (; dist < 0; ++dist) --It;
  }
}

} // end of namespace advance_v2

TEST(iterators, function_dispatch_test) {
  std::vector<int> val = {1, 2, 3, 4, 5, 6};
  auto vi = val.begin();
  advance_v1::adv_v1(vi, 2);
  int v1 = *vi;

  vi = val.begin();
  std::advance(vi, 2);
  int v2 = *vi;

  vi = val.begin();
  advance_v2::adv_v2(vi, 2);
  int v3 = *vi;

  EXPECT_EQ(v1, v2);
  EXPECT_EQ(v3, v2);
}