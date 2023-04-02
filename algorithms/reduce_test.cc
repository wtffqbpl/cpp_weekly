#include <chrono>
#include <gtest/gtest.h>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

#include "internal_check_conds.h"

// std::reduce

#include <execution>

TEST(reduce_test, reduce_basic_test) {
  std::cout.imbue(std::locale("en_US.UTF-8"));
  std::cout << std::fixed << std::setprecision(1);

  auto eval = [](auto fun) {
    const auto t1 = std::chrono::high_resolution_clock::now();
    const auto [name, result] = fun();
    const auto t2 = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> ms = t2 - t1;
    std::cout << std::setw(28) << std::left << name << "sum: " << result
              << "\t time: " << ms.count() << " ms\n";
    return result;
  };

  const std::vector<double> v(100'000'007, 0.1);

  auto accumulate_result = eval([&v] {
    return std::pair{"std::accumulate (double)",
                     std::accumulate(v.cbegin(), v.cend(), 0.0)};
  });
  auto reduce_result = eval([&v] {
    return std::pair{"std::reduce (double)", std::reduce(v.cbegin(), v.cend())};
  });

  EXPECT_EQ(accumulate_result, reduce_result);
}