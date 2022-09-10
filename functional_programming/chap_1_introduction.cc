#include <execution>
#include <fstream>
#include <gtest/gtest.h>
#include <numeric>

std::vector<int> count_lines_in_files(const std::vector<std::string> &files) {
  std::vector<int> results;

  char c;
  for (const auto &file : files) {
    int line_count = 0;
    std::ifstream in(file);

    while (in.get(c)) {
      if (c == '\n')
        ++line_count;
    }

    results.push_back(line_count);
  }

  return results;
}

// FP-style
int count_lines(const std::string &filename) {
  std::ifstream in(filename);
  return std::count(std::istreambuf_iterator<char>(in),
                    std::istreambuf_iterator<char>(), '\n');
}

std::vector<int>
count_lines_in_files_fp(const std::vector<std::string> &files) {
  std::vector<int> results;

  for (const auto &file : files)
    results.push_back(count_lines(file));

  return results;
}

std::vector<int>
count_lines_in_files_lambda(const std::vector<std::string> &files) {
  std::vector<int> results;
  std::transform(files.begin(), files.end(), std::back_inserter(results),
                 [](const auto &file) {
                   std::ifstream in(file);
                   return std::count(std::istreambuf_iterator<char>(in),
                                     std::istreambuf_iterator<char>(), '\n');
                 });

  return results;
}

TEST(fp_programming_introduction, basic_tests) {
  std::string filename = "fp_introduction_basic_tests.txt";
  std::ofstream of{filename};

  for (int i = 0; i < 4000; ++i)
    of << "Hello world\n";
  of.close();

  std::vector<std::string> files = {filename};
  std::vector<int> res = count_lines_in_files_lambda(files);

  EXPECT_EQ(res[0], 4000);
}