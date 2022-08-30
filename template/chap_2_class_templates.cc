#include <cassert>
#include <deque>
#include <gtest/gtest.h>

template <typename T> class Stack {
private:
  std::vector<T> Elems_; // elements.

public:
  Stack(Stack const &) = default; // copy constructor
  // Stack &operator=(Stack const &) = default;                   // first
  // version this is equivalent to Stack<T> &operator=(Stack<T> const &), //
  // second version. #IMPORTANT: It's usually better to use the first form.

public:
  void push(T const &Elem); // push element
  void pop();               // pop element
  T const &top() const;     // return top element.
  bool empty() const {      // return whether the stack is empty.
    return Elems_.empty();
  }

  // io 如果模版涉及到IO，那么需要定义一个 friend operator<< 来实现IO
  template <typename U>
  friend std::ostream &operator<<(std::ostream &, Stack<U> const &) {
    // TODO io declaration examples.
  }
};

template <typename T> void Stack<T>::push(T const &Elem) {
  Elems_.push_back(Elem); // append copy of passed elem.
}

template <typename T> void Stack<T>::pop() {
  assert(!Elems_.empty());
  Elems_.pop_back();
}

template <typename T> T const &Stack<T>::top() const {
  assert(!Elems_.empty());
  return Elems_.back(); // return last element.
}

//################################### Specialization template class
//##################

template <> class Stack<std::string> {
private:
  std::deque<std::string> Elems_; // elements

public:
  void push(std::string const &); // elements
  void pop();                     // pop elements
  std::string const &top() const; // return top element
  bool empty() const {            // return whether the stack is empty
    return Elems_.empty();
  }
};

void Stack<std::string>::push(std::string const &Elem) {
  Elems_.push_back(Elem);
}

void Stack<std::string>::pop() {
  assert(!Elems_.empty());
  Elems_.pop_back();
}

std::string const &Stack<std::string>::top() const {
  assert(!Elems_.empty());
  return Elems_.back(); // return copy of last elements.
}

/*
 * @brief Class templates can be partially specialized. You can provide special
 *        implementations for particular circumstances, but some template
 *        parameters must still be defined by the user.
 */

template <typename T> class Stack<T *> {
private:
  std::vector<T *> Elems_;

public:
  void push(T *);
  T *pop();
  T *top() const;
  bool empty() const { return Elems_.empty(); }
};

template <typename T> void Stack<T *>::push(T *Elem) { Elems_.push_back(Elem); }

template <typename T> T *Stack<T *>::pop() {
  assert(!Elems_.empty());
  T *p = Elems_.back();
  Elems_.pop_back();
  return p;
}

template <typename T> T *Stack<T *>::top() const {
  assert(!Elems_.empty());
  return Elems_.back();
}

// Partial specialization with multiple parameters.
template <typename T1, typename T2> class MyClass {
public:
  explicit MyClass(T1 V1, T2 V2) : V1_(V1), V2_(V2) {}

  T1 getV1() const { return V1_; }
  T2 getV2() const { return V2_; }

private:
  T1 V1_;
  T2 V2_;
};

// partial specialization: second type is int.
template <typename T> class MyClass<T, int> {
public:
  explicit MyClass(T V1, int V2) : V1_(V1), V2_(V2) {}
  T getV1() const { return V1_; }
  int getV2() const { return V2_; }

private:
  T V1_;
  int V2_;
};

// default class template arguments.

// 2.9 Class template Argument Deduction.
template <typename T> class Stack_2_9 {
public:
  Stack_2_9() = default;
  Stack_2_9(T const &elem) // initialize stack with one element
      : elems({elem}) {}

  void printElems() const {
    for (const auto &val : elems)
      std::cout << val << ' ';
    std::cout << '\n';
  }

private:
  std::vector<T> elems; // elemnts.
};

// This allows you to declare a stack as follows.
TEST(chap2_class_template, argument_deduction) {
  Stack_2_9 IntStack = 0; // Stack_2_9<int> deduced since c++17

  std::stringstream oss;
  testing::internal::CaptureStdout();

  IntStack.printElems();
  oss << "0 \n";

  std::string act_output = testing::internal::GetCapturedStdout();
#ifndef NDEBUG
  std::cout << "Expected output\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}
