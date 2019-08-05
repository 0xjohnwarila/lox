#pragma once

#include <fstream>
#include <sstream>

class Lox {
 private:
  bool hadError_;
  std::string path_;
 public:
  Lox(std::string path) {
    this->path_ = path;
    runFile();
  }

  Lox(void) {
    runPrompt();
  }

  void runFile();
  void runPrompt();
  void run(std::string source);
  void error(int line, std::string message);
  void report(int line, std::string where, std::string message);
};

void Lox::runFile() {
  std::ifstream infile(path_);
  std::string line;
  while (getline(infile, line)) {
    run(line);
    if (hadError_) break;
  }
}

void Lox::runPrompt() {
  std::string input;
  for (;;) {
    std::cout << "> ";
    std::cin >> input;
    run(input);
    hadError_ = false;
  }
}