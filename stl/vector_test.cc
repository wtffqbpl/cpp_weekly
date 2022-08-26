#include <deque>
#include <gtest/gtest.h>

TEST(vector, test1) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::vector<int> v = {3, 4, 5, 6};
  auto it = find(v.begin(), v.end(), 4);
  std::cout << "After " << *it << " comes " << *(it + 1) << '\n';
  oss << "After 4 comes 5\n";

  // insert 5 at index 1.
  v.insert(it + 1, 5);
  // 解释: 第一次的capacity = 8, 是因为这里执行了一次insert,
  // 需要对vector进行扩容.

  // remove first value.
  v.erase(v.begin());
  std::cout << "Size = " << v.size() << ", capacity = " << v.capacity() << '\n';
  oss << "Size = 4, capacity = 8\n";

  // clear extra items.
  v.shrink_to_fit();
  std::cout << "Size = " << v.size() << ", capacity = " << v.capacity() << '\n';
  oss << "Size = 4, capacity = 4\n";

  v.push_back(7);

  for (auto i : v)
    std::cout << i << ", ";
  std::cout << '\n';
  oss << "4, 5, 5, 6, 7, \n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "expected output:\n"
            << oss.str() << "\nactual output:\n"
            << act_output << std::endl;
#endif

  EXPECT_EQ(oss.str(), act_output);
}

// 通过emplace 方法，我们可以创建不可复制、不可移动对象的容器.

struct mat {};
struct parameters {};
struct solver {
  solver(const mat &ref, const parameters &para) : ref(ref), para(para) {}
  solver(const solver &) = delete;
  solver(solver &&) = delete;
  void print() const { std::cout << "pass\n"; }

  const mat &ref;
  const parameters &para;
};
void solve_x(const solver &s) { s.print(); }

TEST(vector, emplace_test) {
  parameters p1, p2, p3;
  mat A, B, C;
  std::deque<solver> solvers;

  std::stringstream oss;
  testing::internal::CaptureStdout();

  // solvers.push_back(solver{A, p1}) 无法通过编译
  solvers.emplace_back(B, p1);
  solvers.emplace_back(C, p2);
  solvers.emplace_front(A, p3);

  for (auto &s : solvers)
    solve_x(s);
  oss << "pass\n"
      << "pass\n"
      << "pass\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "expected output:\n"
            << oss.str() << "actual output:\n"
            << act_output << std::endl;
#endif

  EXPECT_TRUE(oss.str() == act_output);
}