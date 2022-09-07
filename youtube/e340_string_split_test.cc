#include <gtest/gtest.h>
// #include <ranges>
#include <string_view>

TEST(string_split, basic_test) {
  auto split_strings = std::string_view{
      "Hello world C++20!"}; // | std::ranges::views::split(' ');
}