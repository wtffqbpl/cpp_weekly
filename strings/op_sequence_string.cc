#include <iostream>
#include <string>
#include <vector>

int main() {
  const std::string delims(" \t,.;");

  // for every line read successfully.
  std::vector<std::string> lines = {
          "pots & pans",
          "I saw a reed",
          "deliver no pets",
          "nametag on diaper"};

  for (auto &line : lines) {
    std::string::size_type begIdx, endIdx;

    // search beginning of the first word.
    begIdx = line.find_first_not_of(delims);

    // while beginning of a word found.
    while (begIdx != std::string::npos) {
      // search end of the actual word.
      endIdx = line.find_first_of(delims, begIdx);
      if (endIdx == std::string::npos) {
        // end of word is end of line.
        endIdx = line.length();
      }

      // print characters in reverse order.
      for (int i = endIdx - 1; i >= static_cast<int>(begIdx); --i)
        std::cout << line[i];
      std::cout << ' ';

      // search beginning of the next word.
      begIdx = line.find_first_not_of(delims, endIdx);
    }

  }

  return 0;
}
