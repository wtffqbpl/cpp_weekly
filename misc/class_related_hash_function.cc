#include <gtest/gtest.h>
#include <unordered_set>

struct Vec3 {
  int x = 0, y = 0, z = 0;

  explicit Vec3(int x = 0, int y = 0, int z = 0) : x(x), y(y), z(z) {}
  Vec3(std::initializer_list<int> &&args) {
    assert(args.size() <= 3 && "Cannot recognize more than 3 arguments case.");
    std::vector<int> vec_args(args.begin(), args.end());
    switch (args.size()) {
    case 3:
      z = vec_args[2];
    case 2:
      y = vec_args[1];
    case 1:
      x = vec_args[0];
      break;
    default:
      assert("Wrong argument size.");
      break;
    }
  }
};
/*
 * @brief define hash function.
 */
struct Vec3HashTraits {
  std::size_t operator()(const Vec3 &V) const {
    return std::size_t(V.x * 1.4514 + V.y * 19 + V.z);
  }
};

/*
 * @brief define equation function.
 */
struct Vec3EqualTraits {
  bool operator()(const Vec3 &v1, const Vec3 &v2) const {
    return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
  }
};

/*
 * @brief 创建hash set.
 * @param type      : 数据类型
 * @param hashfunc  : 哈希函数
 * @param eqfunc    : 相等判断函数
 *
 * std::unordered_set<type, hashfunc, eqfunc> hash;
 */

template <typename... Bases> struct Overload : Bases... {
  using Bases::operator()...; // OK since C++17
};

using CombineOp = Overload<Vec3EqualTraits, Vec3HashTraits>;
using Vec3Set = std::unordered_set<Vec3, CombineOp, CombineOp>;

TEST(class_user_defined_hash, basic_test) {
  Vec3Set pool;

  pool.insert({1, 2, 3});
  pool.insert({1, 2, 3});
  pool.insert({1, 2, 3});
  pool.insert({1, 2, 3});

  EXPECT_EQ(pool.size(), 1);

  pool.insert({5, 6, 7});
  pool.insert({5, 6, 7});
  pool.insert({5, 6, 7});
  pool.insert({5, 6, 7});

  EXPECT_EQ(pool.size(), 2);
}