#ifndef CPP_WEEKLY_NEW_FEATURES_CPP_17_INLINE_VARIABLE_COUNTER_H
#define CPP_WEEKLY_NEW_FEATURES_CPP_17_INLINE_VARIABLE_COUNTER_H

#include <iostream>

namespace inline_variable {

inline int g_counter = 0;

inline void dump() {
  std::cout << "dump: value = " << g_counter << ", "
            << "ptr = " << static_cast<const void *>(&g_counter) << '\n';
  ++g_counter;
}

} // end of namespace inline_variable

namespace inline_variable_initialize {

class Example {
public:
  explicit Example(const char *name) {
    std::cout << "Example(this = " << static_cast<const void *>(name) << ", "
              << "name = \"" << name << "\")\n";
  }
};

} // namespace inline_variable_initialize

#endif // CPP_WEEKLY_NEW_FEATURES_CPP_17_INLINE_VARIABLE_COUNTER_H
