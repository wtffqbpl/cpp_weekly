#include <fstream>
#include <iostream>

void with_io_exceptions(std::ios &io) {
  io.exceptions(std::ios_base::badbit | std::ios_base::failbit);
}

int main() {
  std::ofstream outfile;
  with_io_exceptions(outfile);
  outfile.open("f.txt");

  double o1 = 5.2, o2 = 6.2;
  outfile << o1 << o2 << std::endl;  // no separation.
  outfile.close();

  std::ifstream infile;
  with_io_exceptions(infile);
  infile.open("f.txt");

  int i1, i2;
  char c;
  infile >> i1 >> i2 >> c >> i2;  // does not match type.
  std::cout << "i1 = " << ", i2 = " << i2 << "\n";

  return 0;
}
