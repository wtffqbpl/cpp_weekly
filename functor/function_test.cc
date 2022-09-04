#include <cmath>
#include <gtest/gtest.h>
#include <iostream>
#include <list>

template <typename T>
struct Identity {
  const T &operator()(const T &x) const { return x; }
};

template <typename T1, typename T2>
struct Pair {
  T1 first;
  T2 second;
  using first_type = T1;
  using secnd_type = T2;
  Pair() : first(T1()), second(T2()) {}
  Pair(const T1 &a, const T2 &b) : first(a), second(b) {}
};

template <typename Pair>
struct Select1st {
  const typename Pair::first_type &operator()(const Pair &x) const {
    return x.first;
  }
};

TEST(Functor, Functor_Test1) {
  Identity<int> a;
  int b = a(10);
  EXPECT_EQ(b, 10);
}

/*
 * Unary function
 * STL 将一组需求综合成Unary function的概念:
 *  1. 令 f 的类型是 F;
 *  2. 令 x 的类型是 T, T 是 F 的参数类型;
 *  3. f(x) 以单个参数调用 f, 并以结果类型返回一个对象.
 */

/*
 * calculate       f(x+h) - f(x)
 *            f = --------------
 *                      h
 */
double fin_diff(double F(double), double X, double H) {
  return (F(X + H) - F(X)) / H;
}

double sin_plus_cos(double X) { return std::sin(X) + std::cos(X); }

struct sc_f {
  double operator()(double X) const { return std::sin(X) + std::cos(X); }
};

class psc_f {
public:
  psc_f(double alpha) : Alpha_(alpha) {}
  double operator()(double X) const {
    return std::sin(Alpha_ * X) + std::cos(X);
  }

private:
  double Alpha_;
};

template <typename F, typename T>
T inline fin_diff(F f, const T &X, const T &H) {
  return (f(X + H) - f(X)) / H;
}

TEST(Functor, unary_functor_test) {
  psc_f psc_o(1.0);
  std::cout << fin_diff(psc_o, 1., 0.001) << std::endl;
  std::cout << fin_diff(psc_f(2.0), 1., 0.001) << std::endl;
  std::cout << fin_diff(sin_plus_cos, 0., 0.001) << std::endl;
}

// Composing Functors
//   我们定义一个一元functor, 并将步长和待求导的函数保存为内部状态
template <typename F, typename T> class derivative {
public:
  derivative(const F &f, const T &h) : f(f), h(h) {}

  T operator()(const T &x) const { return (f(x + h) - f(x)) / h; }

private:
  const F &f;
  T h;
};

// 通过这样的定义，x仍然是一个常规的函数参数。这个functor模版可以用一个代表
// f(x) 的functor 来实例化, 实例化的结果是求 f'(x) 近似值的 functor.

using d_psc_f = derivative<psc_f, double>;
// d_psc_f (which indicates calculate f'(x) that is f(x) = sin(a * x) + cos(x)

using dd_psc_f = derivative<d_psc_f, double>;
// dd_psc_f 用来求二阶导数

// 如果有多个函数需要二阶导数，则直接创建二阶导数即可，不用再通过一阶导数来定义
template <typename F, typename T> class second_derivative {
public:
  second_derivative(const F &f, const T &h) : h(h), fp(f, h) {}
  T operator()(const T &x) const { return (fp(x + h) - fp(x)) / h; }

private:
  T h;
  derivative<F, T> fp;
};

// 创建二阶导数的方式
// second_derivative<psc_f, double> dd_psc_2_0(psc_f(1.0), 0.001);

// 如果要实现三阶、四阶甚至任意 n 阶导数，我们就可以利用二阶导数创建的方式类似，
// 以 (x+h) 和 (x) 为参数分别调用 (n-1) 阶的导数。
// 这里用递归的方式来实现n-th阶导数
#if 0
template<typename F, typename T, unsigned N>
class nth_derivative {
  using prev_derivative = nth_derivative<F, T, N - 1>;

public:
  nth_derivative(const F &f, const T &h) : h(h), fp(f, h) {}
  T operator()(const T &x) const {
    return (fp(x + h) - fp(x)) / h;
  }

private:
  T h;
  prev_derivative fp;
};
#endif

// 交叉使用前向和后项差分，能得到更高精度的结果
template <typename F, typename T, unsigned N> class nth_derivative {
  using prev_derivative = nth_derivative<F, T, N - 1>;

public:
  nth_derivative(const F &f, const T &h) : h(h), fp(f, h) {}
  T operator()(const T &x) const {
    return N & 1 ? (fp(x + h) - fp(x)) / h : (fp(x) - fp(x - h)) / h;
  }

private:
  T h;
  prev_derivative fp;
};

// 为了避免编译器无限递归，当抵达一阶导数时要停止递归
#if 0
template<typename F, typename T>
class nth_derivative<F, T, 1> {
public:
  nth_derivative(const F &f, const T &h) : h(h), f(f) {}
  T operator()(const T &x) const {
    return (f(x + h) - f(x)) / h;
  }

private:
  T h;
  const F &f;
};
#endif

// nth_derivative class 和之前derivative是一样的，可以直接模版继承来实现
template <typename F, typename T>
class nth_derivative<F, T, 1> : public derivative<F, T> {
  using derivative<F, T>::derivative;
};

// 计算任意阶导数示例:
// nth_derivative<psc_f, double, 22> d22_psc_o(psc_f(1.0), 0.00001);

class Person {
public:
  explicit Person(std::string lastname, std::string firstname)
      : firstname_(std::move(firstname)), lastname_(std::move(lastname)) {}
  [[nodiscard]] std::string firstname() const { return firstname_; };
  [[nodiscard]] std::string lastname() const { return lastname_; };

private:
  std::string firstname_;
  std::string lastname_;
};

// class for function predicate
class PersonSortTraits {
public:
  bool operator()(const Person &p1, const Person &p2) const {
    return p1.lastname() < p2.lastname() ||
           (p1.lastname() == p2.lastname() && p1.firstname() < p2.firstname());
  }
};

TEST(functor, basic_test_2) {
  std::stringstream oss;
  std::set<Person, PersonSortTraits> coll;

  testing::internal::CaptureStdout();
  coll.insert(Person{"Yuanjun", "Ren"});
  coll.insert(Person{"Xinying", "Li"});

  oss << "Xinying Li\nYuanjun Ren\n";

  for (const auto &p : coll)
    std::cout << p.lastname() << ' ' << p.firstname() << '\n';

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

class IntSequence {
private:
  int value_;

public:
  explicit IntSequence(int initialValue) : value_(initialValue) {}

  int operator()() { return ++value_; }
};

TEST(functor, internal_state_test) {
  std::stringstream oss;
  std::list<int> coll;

  testing::internal::CaptureStdout();

  // insert values from 1 to 9.
  std::generate_n(std::back_inserter(coll), 9, IntSequence(1));

  for (const auto &val : coll)
    std::cout << val << ' ';
  std::cout << '\n';
  oss << "2 3 4 5 6 7 8 9 10 \n";

  // replace second to last element but one with values starting at 42.
  std::generate(std::next(coll.begin()), std::prev(coll.end()),
                IntSequence(42)); // generate values, starting with 42.
  for (const auto &val : coll)
    std::cout << val << ' ';
  std::cout << '\n';
  oss << "2 43 44 45 46 47 48 49 10 \n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}