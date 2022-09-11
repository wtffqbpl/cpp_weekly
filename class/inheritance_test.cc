#include <cmath>
#include <gtest/gtest.h>

class Person {
private:
  std::string Name_;

public:
  Person() = default;
  explicit Person(const std::string &name) : Name_(name) {}
  void set_name(const std::string &name) { Name_ = name; }
  [[nodiscard]] std::string get_name() const { return Name_; }
  void all_info() const {
    std::cout << "[person] My name is " << Name_ << std::endl;
  }
};

class Student : public Person {
private:
  std::string Passed_;

public:
  Student(const std::string &name, const std::string &passed)
      : Person(name), Passed_(passed) {}

  void all_info() const {
    std::cout << "[student] My name is " << get_name() << std::endl;
    std::cout << "I passed the following grades: " << Passed_ << std::endl;
  }
};

TEST(class_test, inheritance_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  Person mark("Yuanjun Ren");
  mark.all_info();
  oss << "[person] My name is Yuanjun Ren\n";

  Student tom("Tom Tomson", "Algebra, Analysis");
  tom.all_info();
  oss << "[student] My name is Tom Tomson\n"
      << "I passed the following grades: Algebra, Analysis\n";

  Person p(tom);
  Person &pr = tom;  // pr(tom) or pr{tom}
  Person *pp = &tom; // pp(&tom) or pp{&tom}

  p.all_info();
  pr.all_info();
  pp->all_info();
  oss << "[person] My name is Tom Tomson\n"
         "[person] My name is Tom Tomson\n"
         "[person] My name is Tom Tomson\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

// 在 C++11 中，允许我们使用 using 声明从基类继承所有构造函数。
class Student_2 : public Person {
  using Person::Person;
};

class Person_3 {
private:
  std::string Name_;

public:
  Person_3() = default;
  explicit Person_3(const std::string &name) : Name_(name) {}
  [[nodiscard]] std::string get_name() const { return Name_; }
  virtual void all_info() const {
    std::cout << "[person] My name is " << Name_ << std::endl;
  }
};

class Student_3 : public Person_3 {
private:
  std::string Passed_;

public:
  explicit Student_3(const std::string &name, const std::string &passed)
      : Person_3(name), Passed_(passed) {}
  std::string get_all_passed() const { return Passed_; }
  void all_info() const override {
    std::cout << "[student] My name is " << get_name() << std::endl;
    std::cout << "I passed the following grades: " << Passed_ << std::endl;
  }
};

TEST(class_test, virtual_inheritance_test_3) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  Person_3 mark("Yuanjun Ren");
  mark.all_info();
  oss << "[person] My name is Yuanjun Ren\n";

  Student_3 tom("Tom Tomson", "Algebra, Analysis");
  tom.all_info();
  oss << "[student] My name is Tom Tomson\n"
      << "I passed the following grades: Algebra, Analysis\n";

  Person_3 p(tom);
  Person_3 &pr = tom;  // pr(tom) or pr{tom}
  Person_3 *pp = &tom; // pp(&tom) or pp{&tom}

  p.all_info();
  pr.all_info();
  pp->all_info();
  oss << "[person] My name is Tom Tomson\n"
      << "[student] My name is Tom Tomson\n"
      << "I passed the following grades: Algebra, Analysis\n"
      << "[student] My name is Tom Tomson\n"
      << "I passed the following grades: Algebra, Analysis\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
};

// Functors via inheritance
struct functor_base {
  virtual double operator()(double x) const = 0;
};

double finite_difference(functor_base const &f, double x, double h) {
  return (f(x + h) - f(x)) / h;
}

class para_sin_plus_cos : public functor_base {
private:
  double alpha;

public:
  explicit para_sin_plus_cos(double p) : alpha(p) {}

  double operator()(double x) const override {
    return std::sin(alpha * x) + std::cos(x);
  }
};

TEST(class_test, functor_inheritance_test) {
  para_sin_plus_cos sin_1{1.0};

  std::cout << finite_difference(sin_1, 1., 0.001) << std::endl;
  double df1 = finite_difference(para_sin_plus_cos(2.), 1., 0.001),
         df0 = finite_difference(para_sin_plus_cos(2.), 0., 0.001);

  std::cout << df0 << ", " << df1 << std::endl;
}

class Mathematician : public Person_3 {
private:
  std::string Proved_;

public:
  explicit Mathematician(const std::string &name, const std::string &proved)
      : Person_3(name), Proved_(proved) {}

  std::string get_proved() const { return Proved_; }
  void all_info() const override {
    Person_3::all_info();
    std::cout << "    I proved: " << Proved_ << std::endl;
  }
};

class MathStudent : public Student_3, public Mathematician {
public:
  explicit MathStudent(const std::string &name, const std::string &passed,
                       const std::string &proved)
      : Student_3(name, passed), Mathematician(name, proved) {}

  void all_info() const override {
    std::cout << "[math student] My name is " << Student_3::get_name() << '\n';
    std::cout << "               I passed the following grades: "
              << Student_3::get_all_passed() << '\n';
    std::cout << "               I proved: " << Mathematician::get_proved()
              << '\n';
  }
};

// Virtual base classes.
class Student_4 : public virtual Person_3 {
private:
  std::string Passed_;

protected:
  explicit Student_4(const std::string &passed) : Passed_(passed) {}

public:
  [[nodiscard]] std::string get_all_passed() const { return Passed_; }
  void all_info() const override {
    std::cout << "[student] My name is " << get_name() << std::endl;
    std::cout << "I passed the following grades: " << Passed_ << std::endl;
  }
};

class Mathematician_4 : public virtual Person_3 {
private:
  std::string Proved_;

protected:
  explicit Mathematician_4(const std::string &proved) : Proved_(proved) {}

public:
  [[nodiscard]] std::string get_proved() const { return Proved_; }
  void all_info() const override {
    Person_3::all_info();
    std::cout << "    I proved: " << Proved_ << std::endl;
  }
};

class MathStudent_4 : public Student_4, public Mathematician_4 {
public:
  MathStudent_4(const std::string &name, const std::string &passed,
                const std::string &proved)
      : Person_3(name), Student_4(passed), Mathematician_4(proved) {}

  void all_info() const override {
    std::cout << "[math student] My name is " << Person_3::get_name() << '\n';
    std::cout << "               I passed the following grades: "
              << Student_4::get_all_passed() << '\n';
    std::cout << "               I proved: " << Mathematician_4::get_proved()
              << '\n';
  }
};

TEST(class_test, virtual_inheritance_common_base_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  MathStudent bob{"Robert Roboson", "Algebra", "Fermat's Last Theorem"};
  bob.all_info();
  oss << "[math student] My name is Robert Roboson\n"
         "               I passed the following grades: Algebra\n"
         "               I proved: Fermat's Last Theorem\n";

  MathStudent_4 bob_4{"Robert Roboson", "Algebra", "Fermat's Last Theorem"};
  bob_4.all_info();
  oss << "[math student] My name is Robert Roboson\n"
         "               I passed the following grades: Algebra\n"
         "               I proved: Fermat's Last Theorem\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

// CRTP --- Curiously Recurring Template Pattern --- 奇异递归模版模式,
// 有效地结合
//          了模版编程和继承。这个术语有时会和 Barton-nackman Trick
//          混淆，后者是基于 CRTP 构造.

class Point {
private:
  int x, y;

public:
  Point() = delete;
  explicit Point(int x = 0, int y = 0) : x(x), y(y) {}

  bool operator==(const Point &that) const {
    return x == that.x && y == that.y;
  }
  bool operator!=(const Point &that) const { return !(*this == that); }
};

template <typename T> struct Inequality {
  bool operator!=(const T &that) const {
    return !(static_cast<const T &>(*this) == that);
  }
};

class Point_2 : public Inequality<Point_2> {
private:
  int x, y;

public:
  Point_2() = delete;
  explicit Point_2(int x = 0, int y = 0) : x(x), y(y) {}

  bool operator==(const Point_2 &that) const {
    return this->x == that.x && this->y == that.y;
  }
};

TEST(inheritance_test, CRTP_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  Point_2 p1{3, 4}, p2{3, 5};

  std::cout << "p1 != p2 is " << std::boolalpha << (p1 != p2) << '\n';
  oss << "p1 != p2 is true\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

template <class Derived> struct Base {
  void name() { static_cast<Derived *>(this)->impl(); }
};

struct D1 : public Base<D1> {
  void impl() { std::cout << "D1::impl()\n"; }
};
struct D2 : public Base<D2> {
  void impl() { std::cout << "D2::impl()\n"; }
};

TEST(inheritance_test, CRTP_test2) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  Base<D1> b1;
  b1.name();
  Base<D2> b2;
  b2.name();
  oss << "D1::impl()\n"
         "D2::impl()\n";

  D1 d1;
  d1.name();
  D2 d2;
  d2.name();
  oss << "D1::impl()\n"
         "D2::impl()\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

// k可以看出，CRTP模式给D1 和 D2
// 这两个类提供了统一的name函数，但是各自的实现又不相同， 可以称之为静态多态.
// 上面的这个例子有些牵强，在更复杂的场景中，尤其是模版编程中，CRTP提供的明确语义会让代码
// 更明确清晰。

template <class Derived> class BaseNew {
public:
  void PrintType() const {
    std::string_view name = typeid(Derived).name();
    std::cout << name.substr(1, name.size() - 1) << '\n';
  }
};

class Derived1 : public BaseNew<Derived1> {};
class Derived2 : public BaseNew<Derived2> {};

TEST(inheritance_test, CRTP_test3) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  Derived1 d1;
  Derived2 d2;

  d1.PrintType();
  d2.PrintType();
  oss << "Derived1\n"
         "Derived2\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

template <typename Matrix, typename Result> class bracket_proxy {
private:
  Matrix &A;
  size_t r;

public:
  explicit bracket_proxy(Matrix &A, size_t r) : A(A), r(r) {}
  Result &operator[](size_t c) { return A(r, c); }
};

template <typename Matrix, typename Result> class crtp_matrix {
  using const_proxy = bracket_proxy<const Matrix, const Result>;

public:
  bracket_proxy<Matrix, Result> operator[](size_t r) {
    return {static_cast<Matrix &>(*this), r};
  }

  const_proxy operator[](size_t r) const {
    return {static_cast<const Matrix &>(*this), r};
  }
};

class some_matrix {
  bracket_proxy<some_matrix, double> operator[](size_t r) {
    return bracket_proxy<some_matrix, double>(*this, r);
  }
};

class matrix_demo {
private:
  int nrows, ncols;
  std::unique_ptr<double[]> data;

public:
  explicit matrix_demo(int nrows, int ncols)
      : nrows(nrows), ncols(ncols), data(new double[nrows * ncols]) {}

  matrix_demo(const matrix_demo &that)
      : nrows(that.nrows), ncols(that.ncols), data(new double[nrows * ncols]) {
    for (size_t i = 0, size = nrows * ncols; i < size; ++i)
      data[i] = that.data[i];
  }

  matrix_demo &operator=(const matrix_demo &that) {
    assert(nrows == that.nrows && ncols == that.ncols);
    for (size_t i = 0, size = nrows * ncols; i < size; ++i)
      data[i] = that.data[i];
    return *this;
  }

  [[nodiscard]] int num_rows() const { return nrows; }
  [[nodiscard]] int num_cols() const { return ncols; }

  double &operator()(int r, int c) {
    assert(0 <= r && r < nrows && 0 <= c && c < ncols);
    return data[r * ncols + c];
  }

  const double &operator()(int r, int c) const {
    assert(r >= 0 && r < nrows && c >= 0 && c < ncols);
    return data[r * ncols + c];
  }
};

// 相应的，存取矩阵元素的时候需要使用括号
/// \code
/// matrix_demo A{2, 3}, B{3, 2};
/// for (int r = 0, nrows = A.num_rows(); r < nrows; ++r)
///   for (int c = 0, ncols = A.num_cols(); c < ncols; ++c)
///     A(r, c) = B(c, r);
/// \endcode
// 这种会导致矩阵的访问，更像是函数调用，而不是对矩阵元素的访问。

template <long N> struct fibonacci {
  static const long value = fibonacci<N - 1>::value + fibonacci<N - 2>::value;
};

template <> struct fibonacci<1> { static const long value = 1; };
template <> struct fibonacci<2> { static const long value = 1; };

TEST(inheritance_test, fibonacci_tmpl_test) {
  long res = fibonacci<45>::value;

#ifndef NDEBUG
  std::cout << "fibonacci<45>::value = " << res << '\n';
#endif

  EXPECT_EQ(res, 1134903170);
}