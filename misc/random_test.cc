#include <gtest/gtest.h>
#include <random>

std::default_random_engine &global_uring() {
  static std::default_random_engine U{};
  return U;
}

/*
 * @brief 通过重设发生器的seed来使得接下来的数字真正的随机化
 */
void randomize() {
  static std::random_device Rd{};
  global_uring().seed(Rd());
}

/*
 * @brief 返回一个区间[From, To] 之内的int数值
 */
int pick(int From, int To) {
  static std::uniform_int_distribution<> D{};
  // template<class _IntType = int>
  // class uniform_int_distribution { } // the default typename is int
  using param_t = decltype(D)::param_type;
  return D(global_uring(), param_t{From, To});
}

/*
 * @brief 返回一个区间[From, To] 之内的double数值
 */
double pick(double From, double To) {
  static std::uniform_real_distribution<> D{};
  using param_t = decltype(D)::param_type;
  return D(global_uring(), param_t{From, To});
}

TEST(random, basic_test) {
  randomize();
  std::cout << "Now, we roll dice:\n";
  for (size_t Idx = 0; Idx < 15; ++Idx)
    std::cout << pick(1, 6) << std::endl;

  std::cout << "\nLet's roll continuous dice now: ;-)\n";
  for (size_t Idx = 0; Idx < 15; ++Idx)
    std::cout << pick(1.0, 6.0) << std::endl;

  // FIXME: don't know how to test this case.
}