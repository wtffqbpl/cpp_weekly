#include <gtest/gtest.h>

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