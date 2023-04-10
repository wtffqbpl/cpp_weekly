#include "internal_check_conds.h"
#include <gtest/gtest.h>

#include <random>

template <int N> struct is_power_of_2 {
  static constexpr bool value = ((N & (N - 1)) == 0);
};

template <typename KeyT> void Swap(KeyT &a, KeyT &b) {
  KeyT temp = a;
  a = b;
  b = temp;
}

template <typename KeyT, typename CompareOp, int N>
void stableOddEvenSort(std::vector<KeyT> &keys, CompareOp compare_op) {
  static_assert(is_power_of_2<N>::value && "N must be the power of two");

  #pragma unroll
  for (int i = 0; i < N; ++i) {
    #pragma unroll
    for (int j = 1 & i; j < N - 1; j += 2) {
      if (compare_op(keys[j + 1], keys[j])) {
        Swap(keys[j], keys[j + 1]);
      }
    }
  }
}

template <typename KeyT> struct Item {
  KeyT key;
  KeyT value;

  Item() = default;

  Item(KeyT key_, KeyT value_) : key(key_), value(value_) {}
  Item(const Item &item) {
    key = item.key;
    value = item.value;
  }

  bool operator<(const Item &item) const { return key < item.key; }
  bool operator==(const Item &item) const {
    return key == item.key && value == item.value;
  }

  Item &operator=(const Item &item) {
    this->key = item.key;
    this->value = item.key;
    return *this;
  }

  template <typename Container,
            int RANGE_START = std::numeric_limits<char>::min(),
            int RANGE_END = std::numeric_limits<char>::max()>
  static void random_init(Container &data) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(RANGE_START,
                                                                  RANGE_END);
    for (auto &val : data) {
      val.key = static_cast<KeyT>(dist(rng));
      val.value = static_cast<KeyT>(dist(rng));
    }
  }
};

template <typename KeyT> struct Compare {
  bool operator()(const KeyT &a, const KeyT &b) const { return a < b; }
};

TEST(odd_evel_sort, basic_test) {
  using KeyT = int;
  using ItemT = Item<KeyT>;
  using CompareOp = Compare<ItemT>;

  constexpr int num_items = 256;
  std::vector<ItemT> data(num_items);
  std::vector<ItemT> data_res(num_items);

  ItemT::random_init(data);

  std::copy(data.begin(), data.end(), data_res.begin());

  std::sort(data_res.begin(), data_res.end());

  stableOddEvenSort<ItemT, CompareOp, num_items>(data, CompareOp());

#if 0
  for (const auto &item : data)
    std::cout << "{" << item.key << ", " << item.value << "}\n";
#endif

  auto res = std::equal(data.begin(), data.end(), data_res.begin());

  EXPECT_TRUE(res);
}
