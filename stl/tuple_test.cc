#include <gtest/gtest.h>
#include <tuple>

struct matrix {};
struct vector {
  vector() = default;
  vector(const size_t n) : size(n), data{new int[n]} {}

  size_t size;
  int *data;
};

std::tuple<matrix, vector> lu(const matrix &A, const size_t n) {
  matrix LU{A};
  vector p{n};

  // ... some calculations.

  // return std::tuple<matrix, vector>(LU, p);
  return {LU, p};
}

// 使用辅助函数make_tuple 可以推导出返回值的类型以简化return语句

std::tuple<matrix, vector> lu_make_tuple(const matrix &A, const size_t n) {
  matrix LU{A};
  vector p{n};

  return std::make_tuple(LU, p);
}

// 函数lu调用方，可以使用get方法来从tuple中提取matrix & vector.
void tuple_test(const matrix &A, const size_t n) {
  // std::tuple<matrix, vector> t = lu_make_tuple(A, n);
  // matrix LU = std::get<0>(t);
  // vector p = std::get<1>(t);

  // 这里可以使用auto来自动进行类型推导
  // auto t = lu_make_tuple(A, n);
  // auto LU = get<0>(t);
  // auto p = get<1>(t);

  // auto LU = get<matrix>(t);
  // auto p = get<vector>(t);

  // auto LU = get<matrix>(std::move(t));
  // auto p = get<vector>(std::move(t));

  matrix LU;
  vector p{};
  std::tie(LU, p) = lu_make_tuple(A, n);
}
