#include <gtest/gtest.h>
#include <regex>

TEST(match_search, basic_test) {
  bool found;
  // find XML/HTML-tagged value (using default syntax).
  std::regex reg1("<.*>.*</.*>");
  found = std::regex_match("<tag>value</tag>", // data
                           reg1);              // regular expression
  EXPECT_TRUE(found);

  // find XML/HTML-tagged value (tags before and after the value must match):
  std::regex reg2("<(.*).*</\\1>");
  found = std::regex_match("<tag>value</tag>", // data
                           reg2);              // regular expression.
  // 这里使用grouping的概念, 我们以(...) 定义出所谓的 capture group,
  // 后面可以借由 正则表达式\1 再次指代它。 c++中的raw string 的表示方式:
  //      R"(<(.*)>.*</\1>)" // equivalent to :"<(.*).*</\\1>"
  // 这样一种 raw string 允许你在定义字符序列时准确地写出其内容。
  EXPECT_TRUE(found);

  // find XML/HTML-tagged value (using grep syntax).
  std::regex reg3(R"(<\(.*\)>.*</\1>)", std::regex_constants::grep);
  found = std::regex_match("<tag>value</tag>", // data
                           reg3);              // regular expression
  EXPECT_TRUE(found);

  // use C-string as regular expression (needs explicit cast to regex).
  found = std::regex_match("<tag>value</tag>",            // data
                           std::regex("<(.*)>.*</\\1>")); // regular expression
  EXPECT_TRUE(found);

  // regex_match() vs. regex_search()
  found = std::regex_match("XML tag: <tag>value</tag>",   // data
                           std::regex("<(.*)>.*</\\1>")); // regular expression
  EXPECT_FALSE(found);

  found =
      std::regex_match("XML tag: <tag>value</tag>",     // data
                       std::regex(".*<(.*)>.*</\\1>")); // regular expression
  EXPECT_TRUE(found);

  found = std::regex_search("XML tag: <tag>value</tag>",   // data
                            std::regex("<(.*)>.*</\\1>")); // regular expression
  EXPECT_TRUE(found);

  found =
      std::regex_search("XML tag: <tag>value</tag>",     // data
                        std::regex(".*<(.*)>.*</\\1>")); // regular expression
  EXPECT_TRUE(found);
}
