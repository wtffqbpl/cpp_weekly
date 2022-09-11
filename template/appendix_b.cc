#include <gtest/gtest.h>

// VALUE CATEGORIES SINCE C++11
//                     expression
//                     /       \
//                   /          \
//            glvalue           rvalue
//           /      \           /    \
//         /         \        /       \
//     lvalue          xvalue        prvalue

// * glvalue: is an expression whose evaluation determines the identity of an
//              object, bit-field, or function.
// * prvalue: is an