#pragma once
#include <gtest/gtest.h>
#include <string>
#include <iostream>

static inline void debug_msg(const std::stringstream &oss,
                             std::string &act_output) {
#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif
}