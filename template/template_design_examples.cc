#include "internal_check_conds.h"
#include <gtest/gtest.h>
#include <iostream>
#include <type_traits>

namespace mtl {
template <typename T> class dense2D {
public:
  using value_type = T;
  using size_type = std::size_t;

  dense2D() = default;

  dense2D(size_type max_r, size_type max_c) : row_(max_r), column_(max_c) {
    matrix_.resize(row_);
    for (auto &row_vec : matrix_)
      row_vec.resize(column_);
  }

  dense2D(const std::initializer_list<std::initializer_list<T>> &list) {
    for (auto &line : list)
      matrix_.emplace_back(line);
    row_ = matrix_.size();
    column_ = matrix_[0].size();
  }

  value_type &operator()(size_type r, size_type c) { return matrix_[r][c]; }

private:
  std::vector<std::vector<T>> matrix_;
  size_type row_, column_;
};

template <typename Matrix> class transpose_view {
public:
  using value_type = typename Matrix::value_type;
  using size_type = typename Matrix::size_type;

  explicit transpose_view(Matrix &A) : ref_(A) {}

  value_type &operator()(size_type r, size_type c) { return ref_(c, r); }
  const value_type &operator()(size_type r, size_type c) const {
    return ref_(c, r);
  }

private:
  Matrix &ref_;
};

template <typename Matrix> inline transpose_view<Matrix> trans(Matrix &A) {
  return transpose_view<Matrix>(A);
}

// If we construct a constant matrix, then the following code would be failed
// during compile-time.
/// \code
/// dense2D<float> A{{2, 3, 4}, {5, 6, 7}, {8, 9, 10}};
/// const dense2D<float> B(A);
/// std::cout << "trans(B)(2, 0) = " << trans(B)(2, 0) << '\n'; // compile
/// failed. \endcode

// An alternative way to solve this problem is that we define a
// const_transpose_view class for const

template <typename Matrix> class const_transpose_view {
public:
  using value_type = typename Matrix::value_type;
  using size_type = typename Matrix::size_type;

  explicit const_transpose_view(const Matrix &A) : ref(A) {}

  const value_type &operator()(size_type r, size_type c) const {
    return ref(c, r);
  }

private:
  const Matrix &ref;
};

template <typename Matrix>
inline const_transpose_view<Matrix> trans(const Matrix &A) {
  return const_transpose_view<Matrix>(A);
}

// Some utilities:
template <typename Matrix> struct transpose_view_f;

// is_matrix declaration
template <typename T> struct is_matrix : std::false_type {};

// specialize template for const specifier.
template <typename T> struct is_matrix<const T> : is_matrix<T> {};

// partial-specialization for matrix
template <typename Value> struct is_matrix<dense2D<Value>> : std::true_type {};

// partial-specialization for transpose_view_f class
template <typename Matrix>
struct is_matrix<transpose_view_f<Matrix>> : is_matrix<Matrix> {};

// This is redundant, because the difference between const_transpose_view and
// transpose_view is no `operator(size_type, size_type) const` function only.

// In this situation, we can use `Compile-time Branching` technique:
// std::conditional The final transpose_view definition:

template <typename Matrix> class transpose_view_f {
  static_assert(is_matrix<Matrix>::value,
                "Argument of this view must be a matrix!");

public:
  using value_type = typename Matrix::value_type;
  using size_type = typename Matrix::size_type;

private:
  using vref_type = std::conditional_t<std::is_const_v<Matrix>,
                                       const value_type &, value_type &>;

public:
  explicit transpose_view_f(Matrix &A) : ref(A) {}

  vref_type operator()(size_type r, size_type c) { return ref(r, c); }
  const value_type &operator()(size_type r, size_type c) const {
    return ref(r, c);
  }

private:
  vref_type ref;
};

} // namespace mtl

namespace variadic_template_type_deduction {
// If you want to deduce the variadic template type, the variadic template
// struct can be used for type deduction. note: variadic template function
// cannot use variadic template parameters type deduction.

// 递归函数和递归类的编译行为是不同的，后者可递归实例化，而前者不行。这就是为什么可以在
// 可变参数类中递归使用 decltype, 但不能在可变参数函数中使用decltype.

template <typename T> inline T sum(T t) { return t; }

/// \code
/// template <typename T, typename... P>
/// auto sum(T t, P... p) -> decltype(t + sum(p...)) // This variadic type
/// deduction
///                                                  // would be failed.
/// {
///   return t + sum(p...);
/// }
/// \endcode

template <typename... P> struct sum_type;

template <typename T> struct sum_type<T> { using type = T; };

template <typename T, typename... P> struct sum_type<T, P...> {
  using type = decltype(T{} + typename sum_type<P...>::type{});
};

template <typename... P> using sum_type_t = typename sum_type<P...>::type;

template <typename T, typename... P>
inline sum_type_t<T, P...> sum_final(T t, P... p) {
  return t + sum(p...);
}

// std::common_type
// https://en.cppreference.com/w/cpp/types/common_type
// Determines the common type among all types T..., that is the type all T...
// can be implicitly converted to. If such a type exists (as determined
// according to the rules below), the member type names that type. Otherwise,
// there is no member type.
//  * if `sizeof...(T)` is zero, there is no member type.
//  * if `sizeof...(T)` is one (i.e., T... contains only one type T0), the
//  member
//    type names the same type as `std::common_type<T0, T0>::type` if it exists;
//    otherwise there is no member type.
//  * If `sizeof...(T) is two:
//      * If applying ...
//
// The types in the parameter pack T shall each be a complete type, void, or an
// array of unknown bound. Otherwise, the behavior is undefined.
// If an instantiation of a template above depends, directly or indirectly, on
// an incomplete type, and that instantiation could yield a different result
// if that type were hypothetically completed, the behavior is undefined.

template <typename T> inline T minimum_my(const T &t) { return t; }

template <typename T, typename... P>
typename std::common_type<T, P...>::type minimum_my(const T &t, const P &...p) {
  using res_type = typename std::common_type<T, P...>::type;
  return std::min(res_type(t), res_type(minimum_my(p...)));
}

template <typename T, typename... P>
inline auto minimum_my_2(const T &t, const P &...p) {
  using res_type = std::common_type_t<T, P...>;
  return std::min(res_type(t), res_type(minimum_my_2(p...)));
}

} // namespace variadic_template_type_deduction
