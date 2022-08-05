#include <iostream>

template <typename T>
struct __my_list_node {
  void *prev;
  void *next;
  T data;
};

template <typename T, typename Ref, typename Ptr>
struct __my_list_iterator {
  typedef __my_list_iterator<T, Ref, Ptr> self;
  typedef Ptr pointer;
  typedef Ref reference;
  typedef __my_list_node<T> *link_type;
  link_type node;
  bool operator==(const self &x) const { return node == x; }
  bool operator!=(const self &x) const { return node != x; }
  reference operator*() const { return (*node).data; }
  pointer operator->() const { return &(operator*()); }
  // &(operator*()) --- 相当于 （&(*iter))
  self &operator++() { node = (link_type)((*node).next); }
  self operator++(int) {
    self tmp = *this;
    ++*this;
    return tmp;
  }
  self &operator--() {
    node = (link_type)((*node).prev);
    return *this;
  }
  self operator--(int) {
    self tmp = *this;
    --*this;
    return *this;
  }
};

int iterator_test_dummy() { return 0; }