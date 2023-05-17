#include <gtest/gtest.h>
#include <memory>

template <typename F, typename T, unsigned N> class nth_derivative {
  using prev_derivative = nth_derivative<F, T, N - 1>;

public:
  nth_derivative(const F &f, const T &h) : h(h), fp(f, h) {}

  T operator()(const T &x) const {
    return N & 1 ? (fp(x + h) - fp(x)) / h : (fp(x) - fp(x - h)) / h;
  }

private:
  T h;
  prev_derivative fp;
};

// To rescue the compiler from infinite recursion, we must stop this mutual
// referring when we reach the first derivative.
template <typename F, typename T> class nth_derivative<F, T, 1> {
public:
  nth_derivative(const F &f, const T &h) : f(f), h(h) {}
  T operator()(const T &x) const { return (f(x + h) - f(x)) / h; }

private:
  const F &f;
  T h;
};

// Reduce redundancy between the functor arguments and those of the
// constructors. auto d7_psc_o = nth_derivative<psc_f, double, 7(psc_o,
// 0.000001); More promising is a make-function that takes the constructor
// arguments and deduces their types like this:
template <typename F, typename T, unsigned N> // Not clever
nth_derivative<F, T, N> make_nth_derivative(const F &f, const T &h) {
  return nth_derivative<F, T, N>(f, h);
}

// As we know, F and T can be deduced during compile time, so we only need to
// declare N explicitly. We have to change the order of our make-function's
// template parameters: N must be declared and F and T can be deduced. Thus,
// we put N in front:
template <unsigned N, typename F, typename T>
nth_derivative<F, T, N> make_nth_derivative_better(const F &f, const T &h) {
  return nth_derivative<F, T, N>(f, h);
}

// We can use this make-function like this(only need to specify N explicitly):
// auto d7_psc_o = make_nth_derivative_better<7>(psc_o, 0.0000001);

// In function templates where all parameters are deduced by the compiler, their
// order is irrelevant. Only when parameters or some of them are explicitly
// declared do we have to pay attention. The parameters not deduced must be
// located at the front of the parameter list.

template <typename T> class odd_range {
public:
  explicit odd_range(const T &start, const T &end) {
    static_assert(std::is_integral_v<T> && "T must be an integral type");

    size_t size = 0;
    if constexpr (std::is_fundamental_v<T>)
      size = end - start;
    else
      size = std::distance(end, start);

    data.resize(size);

    for (auto idx = start; idx < end; ++idx) {
      if ((idx & 2) == 1)
        data.emplace_back(idx);
    }
  }

  using odd_range_iterator = typename std::vector<T>::iterator;
  using const_odd_range_iterator = typename std::vector<T>::const_iterator;

  odd_range_iterator begin() { return data.begin(); }
  odd_range_iterator end() { return data.end(); }

  const_odd_range_iterator begin() const { return data.cbegin(); }
  const_odd_range_iterator end() const { return data.cend(); }

private:
  std::vector<T> data;
};