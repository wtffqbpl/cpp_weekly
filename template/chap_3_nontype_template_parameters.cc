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
