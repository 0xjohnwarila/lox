#include <iostream>

#include <lox.hpp>



int main(int argc, char* argv[]) {
  if (argc > 1) {
    std::cout << "Usage: ./lox [script]" << std::endl;
    return 0;
  }
  else if (argc = 1) {
    Lox(argv[0]);
  }
  else {
    Lox();
  }
  return 0;
}