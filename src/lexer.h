#pragma once

#include <string>
#include <vector>

// Enum of tokens
enum class Token {
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_REDIRECT_APPEND,
    TOKEN_BACKGROUND,
    TOKEN_CD,
};

std::ostream &operator<<(std::ostream &os, Token token);

class Lexer {
  public:
    void Lex(std::string line);
    std::vector<Token> tokens;
    std::vector<std::string> words;
    bool HasBuiltins();

  private:
    Token MatchToken(std::string token);
};
