#include <algorithm>
#include <deque>
#include <gtest/gtest.h>
#include <list>

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

struct value_not_found {};
struct value_not_found_twice {};

template <typename Range, typename Value>
void print_interval(const Range &r, const Value &v,
                    std::ostream &os = std::cout) {
  auto it = std::find(std::begin(r), std::end(r), v), it2 = it;

  if (it == std::end(r))
    throw value_not_found{};
  ++it2;

  auto past = std::find(it2, std::end(r), v);
  if (past == std::end(r))
    throw value_not_found_twice{};
  ++past;

  for (; it != past; ++it)
    os << *it << ' ';
  os << '\n';
}

TEST(stl_range, range_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::list<int> seq = {3, 4, 7, 9, 2, 5, 7, 8};
  print_interval(seq, 7);
  oss << "7 9 2 5 7 \n";

  int array[] = {3, 4, 7, 9, 2, 5, 7, 8};
  std::stringstream ss;
  print_interval(array, 7, ss);
  std::cout << ss.str();
  oss << ss.str();

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << "actual output:\n"
            << act_output << std::endl;
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

// unique 函数在数值处理软件中非常有用，可以删除序列中重复的条目。当然前提是序列必须有序.
// 此时unique会调整序列的条目的位置，把唯一值放在序列的前面，把其他重复的值移到序列的尾部,
// unique 函数的返回值是一个iterator, 指向第一条重复的值, 可以用这个返回值来删除重复
// 的条目。
// std::vector<int> seq = {3, 4, 7, 9, 2, 5, 7, 8, 3, 4, 3, 9};
// std::sort(seq.begin(), seq.end());
// auto it = std::unique(seq.begin(), seq.end());
// seq.resize(std::distance(seq.begin(), it));

// 如果经常使用上述代码，可以把它们封装成一个泛型函数，并用序列作为参数
template <typename Seq>
void make_unique_sequence(Seq &seq) {
  std::sort(std::begin(seq), std::end(seq));
  auto it = std::unique(std::begin(seq), std::end(seq));
  seq.resize(std::distance(std::begin(seq), it));
}

TEST(vector, unique_test) {
  std::vector<int> seq = {3, 4, 7, 9, 2, 5, 7, 8, 3, 4, 3, 9};
  make_unique_sequence(seq);

  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::copy(seq.begin(), seq.end(),
            std::ostream_iterator<int>(std::cout, ", "));
  oss << "2, 3, 4, 5, 7, 8, 9, ";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n" << oss.str() << '\n'
            << "Actual output:\n" << act_output << std::endl;
#endif

  EXPECT_TRUE(oss.str() == act_output);
}