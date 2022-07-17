#include <iostream>
#include <fstream>
#include <sstream>

void write_something(std::ostream &os) {
  os << "Hi stream, did you know that 3 * 3 = " << 3 * 3 << std::endl;
}

int main() {
  std::ofstream myfile("example.txt");
  std::stringstream mysstream;

  write_something(std::cout);
  write_something(myfile);
  write_something(mysstream);

  std::cout << "mystream is: " << mysstream.str();

  return 0;
}


