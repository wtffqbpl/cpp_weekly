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

namespace expression_template {

template <typename T> class VectorSum;

template <typename T> class VectorSum3;

template <typename T> class Vector {
public:
  explicit Vector(int size) : my_size(size), data(new T[my_size]) {}

  const T &operator[](int i) const {
    check_index(i);
    return data[i];
  }

  // ...

  void check_index(int i) {}
  void check_size(int size) {}

  template <typename Src> Vector &operator=(const Src &that) {
    check_size(size(that));
    for (int i = 0; i < my_size; ++i)
      data[i] = that[i];
    return *this;
  }

private:
  friend Vector<T> operator+(const Vector<T> &x, const Vector<T> &y);

private:
  size_t my_size;
  T *data;
};

template <typename T>
inline Vector<T> operator+(const Vector<T> &x, const Vector<T> &y) {
  x.check_index(size(y));
  Vector<T> sum(size(x));

  for (int i = 0; i < size(x); ++i)
    sum[i] = x[i] + y[i];

  return sum;
}

// More efficient add3 implementation:
template <typename T>
void inline add3(const Vector<T> &x, const Vector<T> &y, const Vector<T> &z,
                 Vector<T> &sum) {
  // RVO manually.
  x.check_index(size(y));
  x.check_index(size(z));
  x.check_index(size(sum));

  for (int i = 0; i < size(x); ++i)
    sum[i] = x[i] + y[i] + z[i];
}

// Expression template: builds structures representing a computation at compile
// time, where expressions are evaluated ony as needed to produce efficient for
// the entire computation.

template <typename T> class VectorSum {
public:
  VectorSum(const Vector<T> &v1, const Vector<T> &v2) : v1(v1), v2(v2) {}

  friend int size(const VectorSum &x) { return size(x.v1); }
  // 这个类中最有趣的部分是中括号操作符:
  // 当访问第i个条目的时候，才会计算所有操作数的第i项 之和。
  T operator[](int i) const { return v1[i] + v2[i]; }

  friend VectorSum3<T> inline operator+(const VectorSum<T> &x,
                                        const Vector<T> &y);

private:
  const Vector<T> &v1, &v2;
};

template <typename T>
VectorSum<T> operator+(const Vector<T> &x, const Vector<T> &y) {
  return {x, y};
}

template <typename T> class VectorSum3 {
public:
  VectorSum3(const Vector<T> &v1_, const Vector<T> &v2_, const Vector<T> &v3_)
      : v1(v1_), v2(v2_), v3(v3_) {}

  T operator[](int i) const { return this->v1[i] + this->v2[i] + this->v3[i]; }

private:
  const Vector<T> &v1;
  const Vector<T> &v2;
  const Vector<T> &v3;
};

template <typename T>
VectorSum3<T> inline operator+(const VectorSum<T> &x, const Vector<T> &y) {
  return {x.v1, x.v2, y};
}

template <typename V1, typename V2> struct VectorSumGeneric {
  VectorSumGeneric(const V1 &v1, const V2 &v2) : v1(v1), v2(v2) {}

  using value_type =
      std::common_type_t<typename V1::value_type, typename V2::value_type>;

  value_type operator[](int i) const { return v1[i] + v2[i]; }
  // or (since C++14), compiler will deduce this return type.
  // decltype(auto) operator[](int i) const { return v1[i] + v2[i]; }

private:
  const V1 &v1;
  const V2 &v2;
};

} // namespace expression_template

namespace meta_tuning {
template <typename T, int Size> struct FSizeVector {
  static constexpr int my_size = Size;
  T data[Size];

  template <typename Vector> auto &operator=(const auto &that) {
    for (int i = 0; i < my_size; ++i)
      data[i] = that[i];
    return *this;
  }
};

template <int Rows, int Cols>
struct fsize_mat_vec_mult_cm {
  template <typename Matrix, typename VecIn, typename VecOut>
  void operator()(const Matrix &A, const VecIn &v_in, VecOut &v_out) {
    fsize_mat_vec_mult_cm<Rows - 1, Cols>()(A, v_in, v_out);
    v_out[Rows] += A[Rows] * v_in[Cols];
  }
};

// Specialize for Row = 0
template <int Cols>
struct fsize_mat_vec_mult_cm<0, Cols> {
  template <typename Matrix, typename VecIn, typename VecOut>
  void operator()(const Matrix &A, const VecIn &v_in, VecOut &v_out) {
    fsize_mat_vec_mult_cm<Matrix::my_rows - 1, Cols - 1>()(A, v_in, v_out);
    v_out[0] += A[0][Cols] * v_in[Cols];
  }
};

template <int Rows>
struct fsize_mat_vec_mult_cm<Rows, 0> {
  template <typename Matrix, typename VecIn, typename VecOut>
  void operator()(const Matrix &A, const VecIn &v_in, VecOut &v_out) {
    fsize_mat_vec_mult_cm<Rows - 1, 0>()(A, v_in, v_out);
    v_out[Rows] = A[Rows][0] * v_in[0];
  }
};

template <>
struct fsize_mat_vec_mult_cm<0, 0> {};

} // namespace meta_tuning