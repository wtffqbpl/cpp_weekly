#include <gtest/gtest.h>

template <typename T> // a namespace scope class template
class Data {
public:
  static constexpr bool copyable = true;
};

template <typename T> // a namespace scope function template.
void log(T x) {}

template <typename T> // a namespace scope variable template(since C++14)
T zero = 0;

template <typename T> // a namespace scope variable template(since C++14).
bool dataCopyable = Data<T>::copyable;

template <typename T> // a namespace scope alias template.
using DataList = Data<T *>;

// The following example shows the four kinds of templates as class members
// that are defined within their parent class:
class Collection {
public:
  template <typename T> // an in-class member class template definition.
  class Node {
    //...
  };

  template <typename T> // an in-class (and therefore implicitly inline)
  T *alloc() {          // member function template definition.
    // ...
  }

  template <typename T> // a member variable template (since C++14)
  static T zero = 0;

  template <typename T> // a member alias template.
  using NodePtr = Node<T> *;
};
