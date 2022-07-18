#include <fstream>
#include <iostream>
#include <string>

void write_stream(std::ostream& os, std::string &str) {
  os << str << std::endl;
}

int main() { 
  std::ofstream output_file_hdl("output.log");

  int age = 0;
  std::cout << "What is your name? ";
  std::cin >> age;

  std::string output = "Your age is " + std::to_string(age) + ".";
  write_stream(std::cout, output);
  write_stream(output_file_hdl, output);

  return 0;
}
