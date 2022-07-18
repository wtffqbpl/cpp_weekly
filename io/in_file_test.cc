#include <iostream>
#include <fstream>

int main() {
  std::ifstream infile;
  std::string filename{"some_missing_file.xyz"};
  bool opened = false;

  while (!opened) {
    std::cout << "Open filename " << filename << "...\n";
    infile.open(filename);
    if (infile.good()) {
      opened = true;
    } else {
      std::cout << "The file '" << filename
                << "' doesn't exist, give a new file name: ";
      std::cin >> filename;
    }
  }

  int i;
  double d;
  infile >> i >> d;

  if (infile.good())
    std::cout << "i is " << i << ", d is " << d << "\n";
  else
    std::cout << "Could not correctly read the content.\n";
  infile.close();

  return 0;
}
