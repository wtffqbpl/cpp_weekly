#include <iostream>
#include <iomanip>

int main() {
  double pi = 3.141592653;

  // pi is 3.14159
  std::cout << "pi is " << pi << "\n";
  // pi is 3.141592653
  std::cout << "pi is " << std::setprecision(16) << pi << std::endl;

  // pi is                    3.141592653
  std::cout << "pi is " << std::setw(30) << pi << std::endl;

  // pi is 3.141592653-------------------
  std::cout << "pi is " << std::setfill('-') << std::left << std::setw(30) << pi
            << std::endl;

  // pi is +3.1415926530000000e+00
  std::cout.setf(std::ios_base::showpos);
  std::cout << "pi is " << std::scientific << pi << "\n";

  // 63 octal is 77.
  std::cout << "63 octal is " << std::oct << 63 << ".\n";
  // 63 hexadecimal is 3f.
  std::cout << "63 hexadecimal is " << std::hex << 63 << ".\n";
  // 63 decimal is +63.
  std::cout << "63 decimal is " << std::dec << 63 << ".\n";

  return 0;
}
