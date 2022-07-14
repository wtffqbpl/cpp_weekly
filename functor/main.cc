#include <iostream>

template <typename T>
struct Identity {
  const T &operator()(const T &x) const { return x; }
};

template <typename T1, typename T2>
struct Pair {
  T1 first;
  T2 second;
  using first_type = T1;
  using secnd_type = T2;
  Pair() : first(T1()), second(T2()) {}
  Pair(const T1 &a, const T2 &b) : first(a), second(b) {}
};

template <typename Pair>
struct Select1st {
  const typename Pair::first_type &operator()(const Pair &x) const {
    return x.first;
  }
};

int main() {
  Identity<int> a;
  int b = a(10);
  std::cout << "b(Identity<int>(10)) = " << b << std::endl;

  return 0;
}
