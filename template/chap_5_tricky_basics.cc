#include <deque>
#include <gtest/gtest.h>
#include <utility>

template <typename T> class MyClass {
public:
  MyClass() = default;

  void foo() { typename T::SubTpye *ptr; }
};

// Here, the second typename is used to clarify that SubType is a type defined
// within class T. Thus, ptr is a pointer to the type T::SubType.
// Without typename, SubType would be assumed to be a none-type member (e.g. a
// static data member or an enumerator constant). As a result, the expression
// T::SubType* ptr
// would be a multiplication of the static SubType member of class T with ptr,
// which is not an error, because for some instantiations of MyClass<> this
// could be valid code.
// In general, typename has to be used whenever a name that depends on a
// template parameter is a type.

// print elements of an STL container
template <typename T> void printColl(T const &coll) {
  // typename is the declaration to iterators of standard containers
  // in generic code.
  typename T::const_iterator pos; // iterator to iterate over coll.
  auto end(coll.end());           // end position
  for (pos = coll.begin(); pos != end; ++pos)
    std::cout << *pos << ' ';
  std::cout << '\n';
}

TEST(chap_5_tricky_basics, typename_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::vector<int> pool{1, 2, 3, 4, 5, 6, 7, 8};
  printColl(pool);
  oss << "1 2 3 4 5 6 7 8 \n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(act_output == oss.str());
}

template <typename T, int N, int M> bool less(T (&a)[N], T (&b)[M]) {
  for (int i = 0; i < N && i < M; ++i) {
    if (a[i] < b[i])
      return true;
    if (b[i] < a[i])
      return false;
  }
  return N < M;
}

// Provide a function template for string literals since string -> char const*.
template <int N, int M> bool less(char const (&a)[N], char const (&b)[M]) {
  for (int i = 0; i < N && i < M; ++i) {
    if (a[i] < b[i])
      return true;
    if (b[i] < a[i])
      return false;
  }
  return N < M;
}

TEST(chap_5_tricky_basics, raw_array_template_test) {
  int x[] = {1, 2, 3};
  int y[] = {1, 2, 3, 4, 5};

  EXPECT_TRUE(less(x, y));
  EXPECT_FALSE(less("abc", "ab"));
}

template <typename T> struct MyClass; // primary template

template <typename T, std::size_t SZ>
struct MyClass<T[SZ]> { // partial specialization for arrays of known bounds.
  static void print() { std::cout << "print() for T[" << SZ << "]\n"; }
};

template <typename T, std::size_t SZ> struct MyClass<T (&)[SZ]> {
  // partial spec. for references to arrays of known bounds.
  static void print() { std::cout << "print() for T(&)[" << SZ << "]\n"; }
};

template <typename T>
struct MyClass<T[]> { // partial spec. for arrays of unknown bounds.
  static void print() { std::cout << "print() for T[]\n"; }
};

template <typename T> struct MyClass<T (&)[]> {
  // partial spec. for references to arrays of unknown bounds.
  static void print() { std::cout << "print() for T(&)[]\n"; }
};

template <typename T> struct MyClass<T *> {
  static void print() { std::cout << "print() for T*\n"; }
};

template <typename T1, typename T2, typename T3>
void foo(int a1[7], int a2[],     // pointers by language rules
         int (&a3)[42],           // reference to array of known bound
         int (&x0)[],             // reference to array of unknown bound
         T1 x1,                   // passing by value decays
         T2 &x2, T3 &&x3) {       // passing by reference
  MyClass<decltype(a1)>::print(); // uses MyClass<T*>
  MyClass<decltype(a2)>::print(); // uses MyClass<T*>
  MyClass<decltype(a3)>::print(); // uses MyClass<T(&)[SZ]>
  MyClass<decltype(x0)>::print(); // uses MyClass<T(&)[]>
  MyClass<decltype(x1)>::print(); // uses MyClass<T*>
  MyClass<decltype(x2)>::print(); // uses MyClass<T(&)[]>
  MyClass<decltype(x3)>::print(); // uses MyClass<T(&)[]>
}

TEST(chap_5_tricky_basics, specialize_for_diff_types) {
  std::stringstream oss;

  int a[42];
  extern int x[]; // forward declare array

  testing::internal::CaptureStdout();
  MyClass<decltype(a)>::print(); // uses MyClass<T[SZ]>
  MyClass<decltype(x)>::print(); // uses MyClass<T[]>
  foo(a, a, a, x, x, x, x);
  std::string act_output = testing::internal::GetCapturedStdout();

  oss << "print() for T[42]\n"
         "print() for T[]\n"
         "print() for T*\n"
         "print() for T*\n"
         "print() for T(&)[42]\n"
         "print() for T(&)[]\n"
         "print() for T*\n"
         "print() for T(&)[]\n"
         "print() for T(&)[]\n";

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}
int x[] = {0, 8, 15};

template <typename T> class Stack_5_5 {
private:
  std::deque<T> elems; // elements

public:
  void push(T const &e) { elems.push_back(e); }
  void pop() { elems.pop_front(); }
  [[nodiscard]] T const &top() const { return elems.front(); }
  [[nodiscard]] bool empty() const { return elems.empty(); }

  using TIterator = typename std::deque<T>::iterator;

  TIterator begin() { return elems.begin(); }
  TIterator end() { return elems.end(); }

  // assign stack of elements of type T2.
  template <typename T2> Stack_5_5 &operator=(Stack_5_5<T2> const &);
  // to get access to private members of Stack<T2> for any type T2.
  template <typename> friend class Stack_5_5;
};

#if 0
template <typename T>
template <typename T2>
Stack_5_5<T> &Stack_5_5<T>::operator=(const Stack_5_5<T2> &op2) {
  Stack_5_5<T2> tmp(op2);

  elems.clear();
  while (!tmp.empty()) {
    elems.push_back(tmp.top());
    tmp.pop();
  }

  return *this;
}
#else
/// to get access to private members of Stack<T2> for any type T2.
/// \code
/// template <typename> friend class Stack_5_5;
/// \endcode
template <typename T>
template <typename T2>
Stack_5_5<T> &Stack_5_5<T>::operator=(const Stack_5_5<T2> &op2) {
  elems.clear();
  elems.insert(elems.begin(), op2.elems.begin(), op2.elems.end());
  return *this;
}
#endif

TEST(chap_5_tricky_basics, class_member_template_test) {
  std::stringstream oss;

  Stack_5_5<int> intStack;
  Stack_5_5<float> floatStack;

  for (int i = 0; i < 10; ++i) {
    intStack.push(i);
    floatStack.push(float(i) * 5.4f);
  }

  testing::internal::CaptureStdout();
  for (const auto &val : intStack)
    std::cout << val << ' ';
  std::cout << '\n';
  oss << "0 1 2 3 4 5 6 7 8 9 \n";

  for (const auto &val : floatStack)
    std::cout << val << ' ';
  std::cout << '\n';
  oss << "0 5.4 10.8 16.2 21.6 27 32.4 37.8 43.2 48.6 \n";

  intStack = floatStack;

  for (const auto &val : intStack)
    std::cout << val << ' ';
  std::cout << '\n';
  oss << "0 5 10 16 21 27 32 37 43 48 \n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

template <typename T, typename Cont = std::deque<T>> class Stack_5_5_2 {
private:
  Cont elems;

public:
  void push(T const &elem) { elems.push_front(elem); }
  void pop() { elems.pop_front(); }
  T const &top() const { return elems.front(); }
  [[nodiscard]] bool empty() const { return elems.empty(); }

  using TIterator = typename Cont::iterator;

  TIterator begin() { return elems.begin(); }
  TIterator end() { return elems.end(); }

  // assign stack of elements of type T2.
  template <typename T2, typename Cont2>
  Stack_5_5_2 &operator=(Stack_5_5_2<T2, Cont2> const &);
  // to get access to private members of Stack<T2> for any type T2.
  template <typename, typename> friend class Stack_5_5_2;
};

template <typename T, typename Cont>
template <typename T2, typename Cont2>
Stack_5_5_2<T, Cont> &
Stack_5_5_2<T, Cont>::operator=(const Stack_5_5_2<T2, Cont2> &op2) {
  elems.clear();
  elems.insert(elems.begin(), op2.elems.begin(), op2.elems.end());
}

// Member function templates can also be partially or fully specialized.
class BoolString {
private:
  std::string value_;

public:
  explicit BoolString(std::string s) : value_(std::move(s)) {}

  template <typename T = std::string> [[nodiscard]] T get() const {
    return value_;
  } // get value (converted to T).
};

template <> inline bool BoolString::get<bool>() const {
  return value_ == "true" || value_ == "1" || value_ == "on";
}

TEST(chap_5_tricky_basics, specialization_member_function_test) {
  std::stringstream oss;

  testing::internal::CaptureStdout();
  std::cout << std::boolalpha;
  BoolString s1("hello");
  std::cout << s1.get() << '\n';
  std::cout << s1.get<bool>() << '\n';
  BoolString s2("on");
  std::cout << s2.get<bool>() << '\n';
  std::string act_output = testing::internal::GetCapturedStdout();

  oss << "hello\nfalse\ntrue\n";

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

// Template member functions can be used wherever special member functions
// allow copying or moving objects. Similar to assignment operators as defined
// above, they can also be constructors. However, note that template
// constructors or template assignment operators don't replace predefined
// constructors or assignment operators. member templates don't count as the
// special member functions that copy or move objects. In this example, for
// assignments of stacks of the same type, the default assignment operator
// is still called.
// This effect can be good and bad:
// * It can happen that a template constructor or assignment operator is a
//    better match than the pre-defined copy/move constructor or assignment
//    operator, although a template version is provided for initialization
//    of other types only.
// * It is not easy to 'templify' a copy/move constructor, for example, to be
//    able to constrain its existence.

template <typename T = long double> constexpr T pi{3.1415926535897932385};

// Variable templates can also be parameterized by non-type parameters, which
// also may be used to parameterize the initializer.
template <int N>
std::array<int, N> arr{}; // array with N elements, zero-initialized.
template <auto N>
constexpr decltype(N) dval = N; // type of dval depends on passed value

TEST(chap_5_tricky_basics_class, variable_template_test) {
  std::stringstream oss;

  testing::internal::CaptureStdout();
  std::cout << pi<double> << '\n';
  std::cout << pi<float> << '\n';
  std::cout << pi<> << '\n';
  oss << "3.14159\n3.14159\n3.14159\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

template <typename T> class MyTestClass {
public:
  static constexpr int max = 1000;
};

// variable template allows you to define different values for different
// specializations of MyTestClass<>, then you can define
template <typename T> int myMax = MyTestClass<T>::max;
// so that application programmers can just write
auto i = myMax<std::string>;
// instead of
auto ii = MyTestClass<std::string>::max;
// this means, for a standard class such as
namespace my_std {
template <typename T> class numeric_limits {
public:
  static constexpr bool is_signed = false;
};
} // end of namespace my_std
// you can define
template <typename T>
constexpr bool isSigned = my_std::numeric_limits<T>::is_signed;
// to be able to write
// isSigned<char>;
// instead of
// my_std::numeric_limits<char>::is_signed;

/// Type Traits Suffix _v
/// Since C++17, the standard library uses the technique of variable templates
/// to define shortcut for all type traits in the standard library that yield
/// a (Boolean) value. For example, to be able to write
///
/// \code
/// std::is_const_v<T>; // since C++17
/// \endcode
/// instead of
/// \code
/// std::is_const<T>::value; // since C++11
/// \endcode
/// the standard library defines
/// \code
/// namespace std {
///   template<typename T> constexpr bool is_const_v = is_const<T>::value;
/// }
/// \endcode

// template template parameters.

template <typename T,
          template <typename Elem> typename Cont =
              std::deque> // ERROR before C++17
class Stack_5_7 {
private:
  Cont<T> elems;

public:
  void push(T const &elem) { elems.push_front(elem); }
  void pop() { elems.pop_front(); }
  [[nodiscard]] T const &top() const { return elems.front(); }
  [[nodiscard]] bool empty() const { return elems.empty(); }
};

// Default template arguments of template template arguments were not
// considered, so that a match couldn't be achieved by leaving out arguments
// that have default values(in C++17, default arguments are considered).
// So before C++17, there is a workaround as follows. We can rewrite the class
// declaration so that the Cont parameter expects containers with two template
// parameters.
template <typename T,
          template <typename Elem, typename Alloc = std::allocator<Elem>>
          class Cont = std::deque>
class Stack_before_cpp_17 {
private:
  Cont<T> elems;
};

template <typename T, template <typename Elem, typename = std::allocator<Elem>>
                      class Cont = std::deque>
class StackFinal {
private:
  Cont<T> elems;

public:
  void push(T const &elem);
  void pop();
  [[nodiscard]] T const &top() const;
  [[nodiscard]] bool empty() const { return elems.empty(); }

  using IIterator = typename Cont<T>::iterator;

  IIterator begin() { return elems.begin(); }
  IIterator end() { return elems.end(); }

  // assign stack of elements of type T2
  template <typename T2,
            template <typename Elem2, typename = std::allocator<Elem2>>
            class Cont2>
  StackFinal<T, Cont> &operator=(StackFinal<T2, Cont2> const &);
  // to get access to private members of any Stack with elements of type T2.
  template <typename, template <typename, typename> class>
  friend class StackFinal;
};

template <typename T, template <typename, typename> class Cont>
void StackFinal<T, Cont>::push(const T &elem) {
  elems.push_back(elem);
}

template <typename T, template <typename, typename> class Cont>
void StackFinal<T, Cont>::pop() {
  assert(!elems.empty());
  elems.pop_back();
}

template <typename T, template <typename, typename> class Cont>
T const &StackFinal<T, Cont>::top() const {
  assert(!elems.empty());
  return elems.back();
}

template <typename T, template <typename, typename> class Cont>
template <typename T2, template <typename, typename> class Cont2>
StackFinal<T, Cont> &
StackFinal<T, Cont>::operator=(const StackFinal<T2, Cont2> &op2) {
  elems.clear();
  elems.insert(elems.begin(), op2.elems.begin(), op2.elems.end());
  return *this;
}

TEST(chap_5_tricky_basics, stack_final_test) {
  std::stringstream oss;

  testing::internal::CaptureStdout();
  // manipulate int stack
  StackFinal<int> iStack;
  iStack.push(1);
  iStack.push(2);
  for (const auto &val : iStack)
    std::cout << val << ' ';
  std::cout << '\n';
  oss << "1 2 \n";

  // manipulate float stack:
  StackFinal<float> fStack;
  fStack.push(3.3f);
  for (const auto &val : fStack)
    std::cout << val << ' ';
  std::cout << '\n';
  oss << "3.3 \n";

  // assign stack of different type and manipulate again.
  fStack = iStack;
  fStack.push(4.4f);
  for (const auto &val : fStack)
    std::cout << val << ' ';
  std::cout << '\n';
  oss << "1 2 4.4 \n";

  // Stack for doubles using a vector as an internal container.
  StackFinal<double, std::vector> vStack;
  vStack.push(5.5);
  vStack.push(6.6);
  for (const auto &val : vStack)
    std::cout << val << ' ';
  std::cout << '\n';
  oss << "5.5 6.6 \n";

  vStack = fStack;
  for (const auto &val : vStack)
    std::cout << val << ' ';
  std::cout << '\n';
  oss << "1 2 4.4 \n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}
