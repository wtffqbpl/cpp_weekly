#include <functional>
#include <gtest/gtest.h>
#include <memory>
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

// 11.2.2 std::addressof()
// The std::addressof<>() function template yields the actual address of an
// object or function. It works even if the object type has an overloaded
// operator &. Even though the latter s somewhat rare, it might happen.
// Thus, it is recommended to use addressof() if you need an address of an
// object of arbitrary type:
// 常用于原本要使用operator&
// 的地方，它接受一个参数，该桉树为要获得地址的那个对象的引用。 一般，若operator
// &() 也被重载且不一致的话，那么用 std::addressof 来替代。 std::addressof
// defined in header <memory> You use std::addressof when you have to. Sadly,
// "when you have to" includes anytime you are working in template code and want
// to turn a variable of unknown type T or T& into an honest-to-God pointer to
// that variable's memory.
//
// You should use std::addressof instead of & in any template implementation
// taking the address of a user-defined object. Doing so is not a perfect
// solution as it might cause unexpected behavior with types relying on an
// overloaded & operator (some argu it is an appropriate and just punishment for
// developers dabbling in this dark art). However, it is possible to detect the
// conflict of address reporting and template expectations in debug builds with
// assert (std::addressof(t) == &t).

template <typename T> struct Ptr {
  T *pad;
  T *data;

  explicit Ptr(T *arg) : pad(nullptr), data(arg) {
    std::cout << "Ctor this = " << this << '\n';
  }

  ~Ptr() { delete data; }
  T **operator&() { return &data; }
};

template <typename T> void f(Ptr<T> *p) {
  std::cout << "Ptr overload called with p = " << p << '\n';
}

void f(int **p) { std::cout << "int** overload called with p = " << p << '\n'; }

int test_addressof_1() {
  Ptr<int> p{new int{42}};
  f(&p);                // calls int** overload
  f(std::addressof(p)); // calls Ptr<int>* overload, (= this)

  return 0;
}

// reference: http:// www.cplusplus.com/reference/memory/addressof/
struct unreferneceable {
  int x;
  unreferneceable *operator&() { return nullptr; }
};

void print(unreferneceable *m) {
  if (m)
    std::cout << m->x << '\n';
  else
    std::cout << "[null pointer]\n";
}

int test_addressof_2() {
  void (*pfn)(unreferneceable *) = &print;

  unreferneceable val{10};
  unreferneceable *foo = &val;
  unreferneceable *bar = std::addressof(val);

  (*pfn)(foo); // print [null pointer]
  (*pfn)(bar); // print 10

  return 0;
}

// reference: http://cppisland.com/?p=414
class Buffer {
private:
  static constexpr size_t buffer_size_ = 256;
  int buffer_id_;
  char buffer_[buffer_size_] = {0};

public:
  explicit Buffer(int buffer_id) : buffer_id_(buffer_id) {}
  // BAD practice, only for illustration!
  Buffer *operator&() { return reinterpret_cast<Buffer *>(&buffer_); }
};

template <typename T> void getAddress(T t) {
  std::cout << "Address returned by & operator: " << std::ios::hex << &t
            << '\n';
  std::cout << "Address returned by addressof: " << std::ios::hex
            << std::addressof(t) << '\n';
}

int test_addressof_3() {
  int a = 3;
  fprintf(stderr, "a &: %p, address of: %p\n", &a, std::addressof(a));

  Buffer b{1};
  std::cout << "Getting the address of a Buffer type: \n";
  getAddress(b);

  return 0;
}

// reference: https://wizardforcel.gitbooks.io/beyond-stl/content/38.html
class code_breaker {
public:
  int operator&() const { return 13; }
};

int test_addressof_4() {
  code_breaker c;
  std::cout << "&c: " << (&c) << '\n';
  std::cout << "addressof(t): " << std::addressof(c) << '\n';

  return 0;
}

TEST(chap_11_generic_libraries, addressof_test) {
  test_addressof_1();
  test_addressof_2();
  test_addressof_3();
  test_addressof_4();
}

// 11.3 Perfect forwarding temporaries.
// Sometimes we have to perfectly forward data in generic code that does not
// come through a parameter. In that case, we can use auto&& to create a
// variable that can be forwarded.
// Assume that we have chained calls to functions get() and set() where the
// return value of get() should be perfectly forwarded to set():
/// \code
/// template <typename T>
/// void foo(T x) {
///   set(get(x));
/// }
/// \endcode
// We do this by holding the value in a variable declared with auto&&.
/// \code
/// template <typename T>
/// void foo(T x) {
///   auto&& val = get(x);
///   // perfectly forward the return value of get() to set().
///   set(std::forward<decltype(val)>(val));
/// }
/// \endcode
// This avoids extraneous copies of the intermediate value.

// SUMMARY
//  * Templates allow you to pass functions, function pointers,
//    function objects, functors, and lambdas as callables.
//  * When defining classes with an overloaded operator(), declare it as const
//    (unless the call changes its state).
//  * With std::invoke(), you can implement code that can handle all callables,
//    including member functions.
//  * Use decltype(auto) to forward a return value perfectly.
//  * Type traits are type functions that check for properties and capabilities
//    of types.
//  * Use std::addressof() when you need the address of an object in a template.
//  * Use std::declval() to create values of specific types in unevaluated
//    expressions.
//  * Use auto&& to perfectly forward objects in generic code if their type does
//    not depend on template parameters.
//  * Be prepared to deal with the side effects of template parameters being
//    references.
//  * You can use templates to defer the evaluation of expressions. (e.g. to
//    support using incomplete types in class templates).