#include "internal_check_conds.h"
#include <gtest/gtest.h>

#include <utility>

// CTAD --- Class Template Argument Deduction
// In order to instantiate a class template, every template argument must be
// known, but not every template argument has to be specified.
//
// Deduction for class templates
// Implicitly-generated deduction guides
// When, in a function-style cast or in a variable's declaration, the type
// specifier consists solely of the name of a primary class template C,
// candidates for deduction are formed as follows:
// * If C is defined, for each constructor (or constructor template) Ci
//    declared in the named primary template, a fictional function template
//    Fi, is constructed, such that
//     * template parameters of Fi are the template parameters of C followed
//       (if Ci is a constructor template) by the template parameters of Ci
//       (default template arguments are included too)
//     * the function parameters of Fi are the constructor parameters.
//     * the return type of Fi is C followed by the template parameters of the
//       class template enclosed in <>
// * If C is not defined or does not declare any constructors, an additional
//   fictional function template is added, derived as above from a hypothetical
//   constructor C().
// 在 C++17 之前, 如下所示
//
// std::vector<int> vi{1, 2, 3, 4, 5};
// std::pair<std::vector<int>, int> pvi{vi, 100};
//
// 这里其实我们已经通过vi参数，指定了pair的第一个参数类型为
// std::vector<int>,但是 在c++17 之前, 必须通过上面那种形式显式指定出来.
// 还有类似于下面这种:
//
// std::mutex m_;
// std::lock_guard<std::mutex> lk{m_};
//
// 在C++17 CTAD 中, 可以按照如下来写, 不需要显式指定类型
// std::vector vi{1, 2, 3, 4, 5};
// std::pair pvi{vi, 100};
// std::lock_guard lk{m_};
// C++17 的 deduction guide 格式借助了C++11 的 trailing-return type 标记方法。
// 即将 constructor 当作一个函数，而这个函数要回传一个已经填写好的类别样板
//
// ctor(调用的参数s) -> 期待类型
//

namespace {
template <typename T, typename U> struct my_pair {
  T first;
  U second;

public:
  // #1
  my_pair(T t, U u) : first(t), second(u) {}

  // #2
  template <typename A> explicit my_pair(A a[]) : first(a[0]), second(a[1]) {}
};

// Deduction guide
template <typename A> my_pair(A[]) -> my_pair<A, A>;

my_pair(const char *[])->my_pair<std::string, std::string>;

} // end of namespace

TEST(deduction_guide_test, test1) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  char ca[] = {'a', 'b'};
  my_pair mp2{ca};
  std::cout << "mp2 = {" << mp2.first << ", " << mp2.second << "}\n";
  oss << "mp2 = {a, b}\n";

  const char *str_a[] = {"netflix", "spotify"};
  my_pair mp4{str_a};
  std::cout << "mp4 = {" << mp4.first << ", " << mp4.second << "}\n";
  oss << "mp4 = {netflix, spotify}\n";

  auto act_output = testing::internal::GetCapturedStdout();

  debug_msg(oss, act_output);

  EXPECT_TRUE(oss.str() == act_output);
}

// Deduction Guide for Template Specialization
// 当需要对特化(template
// specialization)后的类别，如果也想做到不指定类型而声明时,

namespace {
template <typename T> struct wrap_ptr;

template <typename T> struct wrap_ptr<T *> {
  explicit wrap_ptr(T *p) : p_(p) {}
  ~wrap_ptr() { delete p_; }

  [[nodiscard]] T get() const { return *p_; }

  int *p_;
};

template <typename T> struct wrap_ptr<std::shared_ptr<T>> {
  explicit wrap_ptr(std::shared_ptr<T> p) : p_(std::move(p)) {}

  [[nodiscard]] T get() const { return *p_; }

  std::shared_ptr<T> p_;
};

template <typename T> wrap_ptr(T *) -> wrap_ptr<T *>;

template <typename T>
wrap_ptr(std::shared_ptr<T>) -> wrap_ptr<std::shared_ptr<T>>;

} // end of namespace

TEST(deduction_guide_test, test2) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  wrap_ptr n_ptr{new int{100}};
  // wrap_ptr<int*>
  std::cout << n_ptr.get() << '\n';
  oss << "100\n";

  std::shared_ptr<int> isp{new int{666}};
  wrap_ptr s_ptr{isp};
  // wrap_ptr<std::shared_ptr<int>>
  std::cout << s_ptr.get() << '\n';
  oss << "666\n";

  auto act_output = testing::internal::GetCapturedStdout();

  debug_msg(oss, act_output);

  EXPECT_TRUE(oss.str() == act_output);
}