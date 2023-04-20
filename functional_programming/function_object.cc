#include <chrono>
#include <gtest/gtest.h>

int ask() { return 42; }
typedef decltype(ask) *function_ptr;

class convertible_to_function_ptr {
public:
  operator function_ptr() const { return ask; }
};

TEST(function_object_test, basic_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  auto ask_ptr = &ask; // 指向函数的指针
  std::cout << ask_ptr() << '\n';
  oss << "42\n";

  auto &ask_ref = ask; // 函数引用
  std::cout << ask_ref() << '\n';
  oss << "42\n";

  convertible_to_function_ptr ask_wrapper; // 可以自动转换成函数指针的对象
  std::cout << ask_wrapper() << '\n';
  oss << "42\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

class person_t {
  int age_;

public:
  explicit person_t(int age) : age_(age) {}
  [[nodiscard]] int age() const { return age_; }
};

class older_than {
  int limit_;

public:
  explicit older_than(const int limit) : limit_(limit) {}

  template <typename T> bool operator()(T &&obj) const {
    return std::forward<T>(obj).age() > limit_;
  }
};

TEST(function_object_test, functor_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::vector<person_t> person = {person_t{1},  person_t{2},  person_t{56},
                                  person_t{24}, person_t{84}, person_t{32},
                                  person_t{31}};

  // count the number of people who is older than 42.
  auto cnt = std::count_if(person.cbegin(), person.cend(), older_than(42));
  EXPECT_EQ(cnt, 2);

  cnt = std::count_if(person.cbegin(), person.cend(), older_than(1));
  EXPECT_EQ(cnt, 6);
}

// mutable
// 此处的意思是，默认lambda使用值捕获时，之后在lambda函数体内，使用被值捕获的变量时，
// 该变量值将永远是其被捕获时，被lambda看到的值，一般这个值是无法改变的。
//
// 如果加上mutable，则会使得该“值捕获变量”的值，可以在被捕获的值的基础上进行变化。
// 且多次调用lambda函数对捕获变量值，所造成的改动会被累积。
// 这是由于lambda函数其实也是一种类，然后下面这个初始化语句：
// auto f=[v1](A a) mutable -> B{...}
// 实际上一方面定义了一种返回值为B，接收一个A类型参数的，这样的lambda函数类型。
// 一方面也定义了一个这样的lambda函数对象 f,
// 所以此处在捕获变量列表中值捕获的变量v1,
// 它其实可以被看做对象f的一个数据成员变量，其初始值为9,
// 因此，函数体内对其每次的变动都会被累积。
TEST(function_object_test, lambda_mutable_test) {
  std::size_t t = 9;
  auto f = [t]() mutable { return ++t; };

  std::cout << f() << '\n';
  std::cout << f() << '\n';

  std::cout << t << '\n';
}

class session_t {
  std::string session_name_;

public:
  explicit session_t(std::string name) : session_name_(std::move(name)) {}
  [[nodiscard]] std::string name() const { return session_name_; }
  friend std::ostream &operator<<(std::ostream &os, const session_t &s) {
    os << s.name();
    return os;
  }
};
TEST(lambda_test, move_type_test) {
  std::unique_ptr<session_t> session(new session_t{"my session"});

  auto on_completed = [session = std::move(session),
                       time = std::chrono::system_clock::now()]() {
    std::cout << "Got response: " << *session << '\n'
              << "The request time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now() - time)
                     .count()
              << "milliseconds";
    return session->name() == "my session";
  };

  EXPECT_TRUE(on_completed());
}

class error_test_t {
  bool error_;

public:
  explicit error_test_t(bool error = true) : error_(error) {}

  template <typename T> bool operator()(T &&value) const {
    return error_ == static_cast<bool>(std::forward<T>(value).error());
  }

  error_test_t operator==(bool test) const {
    return error_test_t(test ? error_ : !error_);
  }
  error_test_t operator!() const { return error_test_t(!error_); }
};
