#include <functional>
#include <gtest/gtest.h>
#include <type_traits>
#include <utility>

// define for_each algorithm.
template <typename Iter, typename Callable>
void for_each_11_1_1(Iter current, Iter end, Callable op) {
  while (current != end) {
    op(*current);
    ++current;
  }
}

void func(int i) { std::cout << "func() called for: " << i << '\n'; }

// define object type (for object that can be used as functions):
class FuncObj {
public:
  void operator()(int i) const {
    std::cout << "FuncObj::op() called for: " << i << '\n';
  }
};

TEST(chap_11_generic_libraries, callable_object_test) {
  std::vector<int> primes = {2, 3, 5, 7, 11, 13, 17, 19};
  // function as callable (decays to pointer).
  for_each_11_1_1(primes.begin(), primes.end(), func);

  // function pointer as callable.
  for_each_11_1_1(primes.begin(), primes.end(), &func);

  // function object as callable.
  for_each_11_1_1(primes.begin(), primes.end(), FuncObj());

  // lambda as callable.
  for_each_11_1_1(primes.begin(), primes.end(), [](int i) {
    std::cout << "lambda called for: " << i << '\n';
  });
}

int f1(int) {
  std::cout << "[int f1(int)] called" << std::endl;
  return true;
}

int f2(char *) {
  std::cout << "[int f2(char *)] called" << std::endl;
  return true;
}

typedef int (*fp1)(int);
typedef int (*fp2)(char *);

struct Caller {
  operator fp1() { return f1; }
  operator fp2() { return f2; }
  int operator()(char *) {
    std::cout << "[int operator()(int)] called" << std::endl;
    return true;
  }

  int call(int) {
    std::cout << "[int call(int)] called" << std::endl;
    return true;
  }
};

TEST(chap_11_generic_libraries, test) {
  Caller call;

  int if1 = call(1); // calls f1 via pointer returned from conversion function
  int if2 = call("this is caller f2"); // calls f2 via pointer returned from
                                       // conversion function

  EXPECT_EQ(if1, if2);
}

// 11.1.2 Dealing with member functions and additional arguments
template <typename Iter, typename Callable, typename... Args>
void for_each_11_1_2(Iter cur, Iter end, Callable op, Args const &...args) {
  while (cur != end) {
    std::invoke(op,      // call passed callable with
                args..., // any additional args
                *cur);   // and the current elements
    ++cur;
  }
}

class MyClass {
public:
  void mem_func(int i) const {
    std::cout << "MyClass::mem_func() called for: " << i << '\n';
  }
};

TEST(chap_11_generic_libraries, member_function_invoke_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::vector<int> coll = {2, 3, 5, 7, 11, 13, 17, 19};
  for_each_11_1_2(
      coll.begin(), coll.end(),
      [](std::string const &prefix, int i) { // lambda to call
        std::cout << prefix << i << '\n';
      },
      "- value: "); // first arg of lambda
  oss << "- value: 2\n"
      << "- value: 3\n"
      << "- value: 5\n"
      << "- value: 7\n"
      << "- value: 11\n"
      << "- value: 13\n"
      << "- value: 17\n"
      << "- value: 19\n";

  // call obj.mem_func() for/with elements in coll passed as argument
  MyClass obj;
  for_each_11_1_2(coll.begin(), coll.end(),
                  &MyClass::mem_func, // member function to call
                  obj);               // object to call mem_func() for

  oss << "MyClass::mem_func() called for: 2\n"
      << "MyClass::mem_func() called for: 3\n"
      << "MyClass::mem_func() called for: 5\n"
      << "MyClass::mem_func() called for: 7\n"
      << "MyClass::mem_func() called for: 11\n"
      << "MyClass::mem_func() called for: 13\n"
      << "MyClass::mem_func() called for: 17\n"
      << "MyClass::mem_func() called for: 19\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

// 11.1.3 Wrapping function calls.
// A common application of std::invoke() is to wrap single function calls.
// e.g.
//  * log the calls
//  * measure their duration
//  * or prepare some context such as starting a new thread for them.

// call impl with move semantics by perfect forwarding both the callable and
// all passed arguments.
template <typename Callable, typename... Args>
decltype(auto) call(Callable &&op, Args &&...args) {
  return std::invoke(std::forward<Callable>(op),   // passed callable with
                     std::forward<Args>(args)...); // any additional args
}

// If you want to temporarily store the value returned by std::invoke() in a
// variable to return it after doing something else (e.g. to deal with the
// return value or log the end of the call), you also have to declare the
// temporary variable with decltype(auto):
template <typename Callable, typename... Args>
decltype(auto) call_temp(Callable &&op, Args &&...args) {
  decltype(auto) res{
      std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...)};

  return res;
}

// decltype(auto): If the callable has return type void, the initialization of
//    ret as decltype(auto) is not allowed, because void is an incomplete type.
// You have the following options:
//  * Declare an object in the line before that statement, whose destructor
//    performs the observable behavior that you want to realize.
/// \code
/// struct cleanup {
///   ~cleanup() {
///      ... // code to perform on return
///   }
/// } dummy;
/// return std::invoke(std::forward<Callable>(op),
///                    std::forward<Args>(args)...);
/// \endcode
//
// * implement the void and non-void cases differently:
template <typename Callable, typename... Args>
decltype(auto) call_void_non_void(Callable &&op, Args &&...args) {
  // NOTE:
  // std::invoke_result<> is available since C++17. Since C++11, to get the
  // return type you could call:
  // typename std::result_of<Callable(Args...)>::type
  if constexpr (std::is_same_v<std::invoke_result<Callable, Args...>, void>) {
    // return type is void.
    std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...);
    // ...
    return;
  } else {
    // return type is not void.
    decltype(auto) ret{
        std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...)};
    // ...
    return ret;
  }
}

// 11.2.1 Type Traits: that allow us to evaluate and modify types.
//  This supports various cases where generic code has to adapt to or react
//  on the capabilities of the types for which they are instantiated.
template <typename T> class C {
  // ensure that T is not void(ignoring const or volatile).
  static_assert(!std::is_same_v<std::remove_cv_t<T>, void>,
                "invalid instantiation of class C for void type.");

public:
  template <typename V> void f(V &&v) {
    if constexpr (std::is_reference_v<T>) {
      // ... special code if T is a reference type.
    }

    if constexpr (std::is_convertible_v<std::decay_t<V>, T>) {
      // special code if V is convertible to T.
    }

    if constexpr (std::has_virtual_destructor_v<V>) {
      // special code if V has virtual destructor.
    }
  }
};
