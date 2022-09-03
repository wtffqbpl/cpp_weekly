#include <gtest/gtest.h>

template <typename T, std::size_t Maxsize> class Stack_3_1 {
private:
  std::array<T, Maxsize> elems; // elements
  std::size_t numElems;         // current number of elements.

public:
  Stack_3_1() : numElems{0} {}
  void push(T const &elem);
  void pop();
  [[nodiscard]] T const &top() const;
  [[nodiscard]] bool empty() const { return numElems == 0; }
  [[nodiscard]] std::size_t size() const { return numElems; }
};

template <typename T, std::size_t Maxsize>
void Stack_3_1<T, Maxsize>::push(T const &elem) {
  assert(numElems < Maxsize);
  elems[numElems++] = elem;
}

template <typename T, std::size_t Maxsize> void Stack_3_1<T, Maxsize>::pop() {
  assert(!elems.empty());
  --numElems;
}

template <typename T, std::size_t Maxsize>
[[maybe_unused]] T const &Stack_3_1<T, Maxsize>::top() const {
  assert(!elems.empty());
  return elems[numElems - 1];
}

TEST(chap_3_nonteyp_template_parameters, nontype_class_params) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  Stack_3_1<int, 20> int20Stack;          // stack of up to 20 ints.
  Stack_3_1<int, 40> int40Stack;          // stack of up to 40 ints.
  Stack_3_1<std::string, 40> stringStack; // stack of up to 40 strings.

  // manipulate stack of up to 20 ints.
  int20Stack.push(7);
  std::cout << int20Stack.top() << '\n';
  int20Stack.pop();
  oss << "7\n";

  // manipulate stack of up to 40 strings.
  stringStack.push("Hello");
  std::cout << stringStack.top() << '\n';
  stringStack.pop();
  oss << "Hello\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

/// 3.2 Nontype Function Template Parameters
template <int Val, typename T> constexpr T addValue(T x) { return x + Val; }

/// To derive the return type from the passed nontype.
template <auto Val, typename T = decltype(Val)> T addValue_2(T x) {
  return x + Val;
}

/// To ensure that the passed value has the same type as the passed type.
template <typename T, T Val = T{}> T addValue_3(T x) { return x + Val; }

TEST(chap_3_nonteyp_template_parameters, nontype_function_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::vector<int> arr1{1, 2, 3, 4, 5, 6};
  std::vector<int> res;

  // case 1;
  std::transform(arr1.begin(), arr1.end(), std::back_inserter(res),
                 addValue<5, int>);
  for (const auto &val : res)
    std::cout << val << ' ';
  std::cout << '\n';
  std::copy(res.begin(), res.end(), std::ostream_iterator<int>(oss, " "));
  oss << '\n';

  // case 2
  res.clear();
  std::transform(arr1.begin(), arr1.end(), std::back_inserter(res),
                 addValue_2<5>);
  for (const auto &val : res)
    std::cout << val << ' ';
  std::cout << '\n';
  std::copy(res.begin(), res.end(), std::ostream_iterator<int>(oss, " "));
  oss << '\n';

  // case 3
  res.clear();
  std::transform(arr1.begin(), arr1.end(), std::back_inserter(res),
                 addValue_3<int, 5>);
  for (const auto &val : res)
    std::cout << val << ' ';
  std::cout << '\n';
  std::copy(res.begin(), res.end(), std::ostream_iterator<int>(oss, " "));
  oss << '\n';

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}