#include <deque>
#include <utility>
#include <gtest/gtest.h>

template <typename T>
class MyClass {
public:
  MyClass() = default;

  void foo() {
    typename T::SubTpye* ptr;
  }
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
template <typename T>
void printColl(T const &coll) {
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

template <typename T, int N, int M>
bool less(T(&a)[N], T(&b)[M]) {
  for (int i = 0; i < N && i < M; ++i) {
    if (a[i] < b[i]) return true;
    if (b[i] < a[i]) return false;
  }
  return N < M;
}

// Provide a function template for string literals since string -> char const*.
template <int N, int M>
bool less(char const(&a)[N], char const(&b)[M]) {
  for (int i = 0; i < N && i < M; ++i) {
    if (a[i] < b[i]) return true;
    if (b[i] < a[i]) return false;
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

template <typename T, std::size_t SZ>
struct MyClass<T(&)[SZ]> {
  // partial spec. for references to arrays of known bounds.
  static void print() { std::cout << "print() for T(&)[" << SZ << "]\n"; }
};

template <typename T>
struct MyClass<T[]> { // partial spec. for arrays of unknown bounds.
  static void print() { std::cout << "print() for T[]\n"; }
};

template <typename T>
struct MyClass<T(&)[]> {
  // partial spec. for references to arrays of unknown bounds.
  static void print() { std::cout << "print() for T(&)[]\n"; }
};

template <typename T>
struct MyClass<T*> {
  static void print() { std::cout << "print() for T*\n"; }
};

template <typename T1, typename T2, typename T3>
void foo(int a1[7], int a2[], // pointers by language rules
         int (&a3)[42],       // reference to array of known bound
         int (&x0)[],         // reference to array of unknown bound
         T1 x1,               // passing by value decays
         T2 &x2, T3 &&x3) {   // passing by reference
  MyClass< decltype(a1)>::print();  // uses MyClass<T*>
  MyClass< decltype(a2)>::print();  // uses MyClass<T*>
  MyClass< decltype(a3)>::print();  // uses MyClass<T(&)[SZ]>
  MyClass< decltype(x0)>::print();  // uses MyClass<T(&)[]>
  MyClass< decltype(x1)>::print();  // uses MyClass<T*>
  MyClass< decltype(x2)>::print();  // uses MyClass<T(&)[]>
  MyClass< decltype(x3)>::print();  // uses MyClass<T(&)[]>
}

TEST(chap_5_tricky_basics, specialize_for_diff_types) {
  std::stringstream oss;

  int a[42];
  extern int x[];                 // forward declare array

  testing::internal::CaptureStdout();
  MyClass< decltype(a)>::print(); // uses MyClass<T[SZ]>
  MyClass< decltype(x)>::print(); // uses MyClass<T[]>
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

template <typename T>
class Stack_5_5 {
private:
  std::deque<T> elems; //elements

public:
  void push(T const &e) { elems.push_back(e); }
  void pop() { elems.pop_front(); }
  [[nodiscard]] T const &top() const { return elems.front(); }
  [[nodiscard]] bool empty() const { return elems.empty(); }

  using TIterator = typename std::deque<T>::iterator;

  TIterator begin() { return elems.begin(); }
  TIterator end() { return elems.end(); }

  // assign stack of elements of type T2.
  template <typename T2>
  Stack_5_5 &operator=(Stack_5_5<T2> const &);
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

template <typename T, typename Cont = std::deque<T>>
class Stack_5_5_2 {
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
Stack_5_5_2<T, Cont>
    &Stack_5_5_2<T, Cont>::operator=(const Stack_5_5_2<T2, Cont2> &op2) {
  elems.clear();
  elems.insert(elems.begin(), op2.elems.begin(), op2.elems.end());
}

// Member function templates can also be partially or fully specialized.
class BoolString {
private:
  std::string value_;

public:
  explicit BoolString(std::string s) : value_(std::move(s)) {}

  template <typename T = std::string>
  T get() const { return value_; } // get value (converted to T).
};

template<> inline bool BoolString::get<bool>() const {
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