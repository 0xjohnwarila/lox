#pragma once

#include <string>

#include "TokenType.hpp"

class Token {
 private:
  TokenType type;
  std::string lexeme;
  
};