#include "counter.h"

namespace inline_variable {

void inline_variable_test() {
  std::cout << "func: value = " << g_counter << ", "
            << "ptr = " << static_cast<const void *>(&g_counter) << '\n';
  dump();
  std::cout << "func: value = " << g_counter << ", "
            << "ptr = " << static_cast<const void *>(&g_counter) << '\n';
}
}; // end of namespace inline_variable

namespace inline_variable_initialize {
inline Example b{"b"};

} // end of namespace inline_variable_initialize
