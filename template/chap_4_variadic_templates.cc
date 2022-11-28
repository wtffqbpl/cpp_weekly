#include <array>
#include <complex>
#include <gtest/gtest.h>
#include <unordered_set>
#include <utility>

template <typename T> void print(T arg) { std::cout << arg << '\n'; }

template <typename T, typename... Types> void print(T firstArg, Types... args) {
  print(firstArg); // call print() for the first argument.
  print(args...);  // call print() for remaining arguments.
}

TEST(chap4_variadic_templates, print_test) {
  std::stringstream oss;

  // Start to capture stdout.
  testing::internal::CaptureStdout();

  print(7.5, "Hello", "world", 18.0);
  oss << "7.5\nHello\nworld\n18\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << "Actual output:\n"
            << act_output << std::endl;
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

template <typename T> void print_sizeof(T firstArg) {
  std::cout << firstArg << ' ';
}

template <typename T, typename... Types>
void print_sizeof(T firstArg, Types... args) {
  // print number of remaining types.
  std::cout << "sizeof...(Types) = " << sizeof...(Types) << '\n';
  // print number of remaining args.
  std::cout << "sizeof...(args) = " << sizeof...(args) << '\n';

  print_sizeof(firstArg);
  // print remaining.
  print_sizeof(args...);
}

TEST(chap4_variadic_templates, sizeof_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  print_sizeof("Hello", "world", 1, 3.4);

  oss << "sizeof...(Types) = 3\n"
         "sizeof...(args) = 3\n"
         "Hello sizeof...(Types) = 2\n"
         "sizeof...(args) = 2\n"
         "world sizeof...(Types) = 1\n"
         "sizeof...(args) = 1\n"
         "1 3.4 ";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

/// Fold Expressions.

/// Since c++17, there is a feature to compute the result of using a binary
/// operator over all the arguments of a parameter pack (with an optional
//  initial value).

// Fold Expressions (since c++17)
// |------------------------------------------------------------------------|
// |  FOLD EXPRESSION           |           EVALUATION                      |
// |------------------------------------------------------------------------|
// |  ( ... op pack )           | (((pack1 op pack2) op pack3)...op packN)  |
// |  ( pack op ... )           | (pack1 op (...(packN-1 op packN)))        |
// |  ( init op ... op pack )   | (((init op pack1) op pack2)...op packN)   |
// |  ( pack op ... op init)    | (pack1 op (...(packN op init)))           |
// -------------------------------------------------------------------------|

template <typename... T> auto foldSum_1(T... s) {
  // ((s1 + s2) + s3)...
  return (... + s);
}

template <typename... T> auto foldSum_2(T... s) {
  // (s1 + (s2 + (s3 + (...)))
  return (s + ...);
}

TEST(chap4_variadic_templates, fold_expression_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  auto sum1 = foldSum_1(1, 2, 3, 4, 5, 6);
  auto sum2 = foldSum_2(1, 2, 3, 4, 5, 6);

  std::cout << "foldSum_1(1, 2, 3, 4, 5, 6) = " << sum1 << '\n';
  std::cout << "foldSum_2(1, 2, 3, 4, 5, 6) = " << sum2 << '\n';

  std::string act_output = testing::internal::GetCapturedStdout();

  oss << "foldSum_1(1, 2, 3, 4, 5, 6) = 21\n"
      << "foldSum_2(1, 2, 3, 4, 5, 6) = 21\n";

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

// define binary tree structure and traverse helpers.
struct Node {
  int value;
  Node *left;
  Node *right;
  explicit Node(int i = 0) : value(i), left(nullptr), right(nullptr) {}
};

auto left = &Node::left;
auto right = &Node::right;

// transverse tree, using folding expression.
template <typename T, typename... TP> Node *traverse(T np, TP... paths) {
  // np ->* paths1 ->* paths2
  // uses a fold expression to traverse the variadic elements of paths from np.
  return (np->*...->*paths);
}

template <typename... Types>
void print_using_fold_expression(Types const &...args) {
  (std::cout << ... << args) << '\n';
}

TEST(chap4_variadic_templates, fold_expression_traverse_test) {
  // init binary tree structure.
  Node *root = new Node{};
  root->left = new Node{1};
  root->left->right = new Node{2};

  // traverse binary tree.
  Node *node = traverse(root, left, right);

  if (node)
    std::cout << node->value << '\n';

  print_using_fold_expression("Hello", "World", 1, 2.3, 15.f, "end");
}

// Folding expression case.

template <typename T> void op(const T &arg) { std::cout << arg << ' '; }
template <typename... Args> void print_test(Args &&...args) {
  (op(args), ...);
  op('\n');
}
template <typename T> class AddSpace {
private:
  T const &ref;

public:
  explicit AddSpace(T const &r) : ref(r) {}
  friend std::ostream &operator<<(std::ostream &os, const AddSpace<T> &Val) {
    os << Val.ref << ' ';
    return os;
  }
};

template <typename... Types> void print_add_space(Types... args) {
  (std::cout << ... << AddSpace(args)) << '\n';
}

TEST(chap4_variadic_templates, fold_expression_print_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  print_test(1, 2, 3, "abc");
  oss << "1 2 3 abc \n ";
  print_add_space(1, 2, 3, "abc");
  oss << "1 2 3 abc \n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

/*
 * @brief push back variadic elements to vector.
 */
template <class T, class... Args>
void push_back_vec(std::vector<T> &v, Args &&...args) {
  (v.push_back(args), ...);
}

TEST(chap4_variadic_templates, fold_expression_push_back_vec) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::vector<int> v;
  push_back_vec(v, 6, 2, 3, 45, 12);

  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  oss << "6 2 3 45 12 ";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

template <typename... T> int sum(T... args) { return (0 + ... + args); }
template <typename... T> int sum2(T... args) { return (args + ...); }
template <typename... T> auto div(T... args) { return (args / ...); }
template <typename... T> auto avg(T... args) {
  return (args + ...) / sizeof...(args);
}

TEST(chap4_variadic_templates, fold_expression_more_cases) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::cout << sum(2, 8, 3, 5, 23, 3) << '\n';
  std::cout << sum2(2, 8, 3, 5, 23, 3) << '\n';
  std::cout << sum(2, 8, 3.3, 5, 23, 3) << '\n';
  std::cout << sum2(2, 8, 3.3, 5, 23, 3) << '\n';

  std::cout << div(3, 2, 1) << '\n';
  std::cout << div(3.0, 2.0, 2.0) << '\n';
  std::cout << avg(1.0, 2.0, 3.0) << '\n';

  std::string act_output = testing::internal::GetCapturedStdout();

  oss << "44\n"
         "44\n"
         "44\n"
         "44\n"
         "1\n"
         "3\n"
         "2\n";

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

// Variadic templates play an important role when implementing generic
// libraries, such as the C++ standard library.
// One typical application is the forwarding of a variadic number of arguments
// of arbitrary type. For exmaple, we use this feature when:
//  * Passing arguments to the constructor of a new heap object owned by a
//    shared pointer:
//      // create shared pointer to complex<float> initialized by 4.2 and 7.7.
//      auto sp = std::make_shared<std::complex<float>>(4.2, 7.7);
//  * Passing arguments to a thread, which is started by the library.
//    std::thread t(foo, 42, "hello");
//  * Passing arguments to the constructor of a new element pushed into
//    a vector.
//    std::vector<Customer> v;
//    ...
//    // insert a Customer initialized by three arguments.
//    v.emplace_back("Tim", "Jove", 1962);
//
// Usually, the arguments are "perfectly forwarded" with move semantics.
// So that the corresponding declarations are, for example:

///
/// \code
/// namespace std {
/// template <typename T, typename... Args>
/// shared_ptr<T> make_shared(Args&&... args);
///
/// class thread {
/// public:
///   template <typename F, typename... Args>
///   explicit thread(F&& f, Args&&... args);
///   ...
/// };
///
/// template <typename T, typename Allocator = std::allocator<T>>
/// class vector {
/// public:
///   template <typename... Args> reference emplace_back(Args&&... args);
///   ...
/// };
/// }
/// \endcode
///

template <typename C, typename... Idx>
void printElems(C const &coll, Idx... idx) {
  print(coll[idx]...);
}

template <std::size_t... Idx, typename C> void printIdx(C const &coll) {
  print(coll[Idx]...);
}

// type for arbitrary number of indices.
template <std::size_t...> struct Indices {};

// Define a function that calls print() for the elements of a std::array or
// std::tuple using the compile-time access with get<>() for the given indices.
// This is equivalent to std::index_sequence
template <typename T, std::size_t... Idx>
void printByIdx(T t, Indices<Idx...>) {
  print(std::get<Idx>(t)...);
}

template <typename T, std::size_t... Idx>
void printByIdx(T t, std::index_sequence<Idx...>) {
  print(std::get<Idx>(t)...);
}

TEST(chap4_variadic_templates, variadic_indices_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::vector<std::string> coll = {"good", "times", "say", "bye"};
  printElems(coll, 2, 0, 3);
  // This is equivalent to print(coll[2], coll[0], coll[3]);
  oss << "say\ngood\nbye\n";

  printIdx<2, 0, 3>(coll);
  oss << "say\ngood\nbye\n";

  std::array<std::string, 5> arr = {"Hello", "my", "new", "!", "World"};
  printByIdx(arr, Indices<0, 4, 3>());
  oss << "Hello\nWorld\n!\n";
  printByIdx(arr, std::index_sequence<0, 4, 3>{});
  oss << "Hello\nWorld\n!\n";

  auto t = std::make_tuple(12, "monkeys", 2.0);
  printByIdx(t, Indices<0, 1, 2>());
  oss << "12\nmonkeys\n2\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

template <typename... T> void printDoubled(T const &...args) {
  print(args + args...);
}

template <typename... T> void addOne(T const &...args) {
  print(args + 1 ...); // OK
  // print((args + 1)...); // OK
}

template <typename T1, typename... TN> constexpr bool isHomogeneous(T1, TN...) {
  return (std::is_same<T1, TN>::value && ...); // since C++17
}

TEST(chap4_variadic_templates, variadic_more_testcases) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  // This is equivalent to
  /// \code
  /// printDoubled(7.5 + 7.5,
  ///              std::string("Hello") + std::string("Hello"),
  ///              std::complex<float>(4, 2) + std::complex<float>(4, 2));
  /// \endcode
  printDoubled(7.5, std::string("Hello"), std::complex<float>(4, 2));
  oss << "15\nHelloHello\n(8,4)\n";

  /// \code
  /// print(3 + 1, 4.5 + 1, 8.f + 1);
  /// \endcode
  addOne(3, 4.5, 8.f);
  oss << "4\n5.5\n9\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);

  /// \code
  /// std::is_same<int, int>::value && std:is_same<int, char const*>::value
  /// \endcode
  EXPECT_FALSE(isHomogeneous(43, -1, "hello"));
  EXPECT_TRUE(isHomogeneous("Hello", " ", "world", "!"));
}

class Customer {
private:
  std::string Name_;

public:
  explicit Customer(std::string name) : Name_(std::move(name)) {}
  [[nodiscard]] std::string getName() const { return Name_; }
};

struct CustomerEq {
  bool operator()(Customer const &C1, Customer const &C2) const {
    return C1.getName() == C2.getName();
  }
};

struct CustomerHash {
  std::size_t operator()(Customer const &C) const {
    return std::hash<std::string>()(C.getName());
  }
};

// define class that combines operator() for variadic base classes:
template <typename... Bases> struct Overloader : Bases... {
  using Bases::operator()...; // OK since C++17
};

TEST(chap4_variadic_templates, variadic_base_classes_and_using_test) {
  // combine hasher and equality for customers in one type.
  using CustomerOP = Overloader<CustomerHash, CustomerEq>;

  std::unordered_set<Customer, CustomerHash, CustomerEq> coll1;
  std::unordered_set<Customer, CustomerOP, CustomerOP> coll2;
}