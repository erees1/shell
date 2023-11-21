#pragma once

#include <string>
#include <vector>

// Enum of tokens
class Token {

  public:
    enum class Type {
        WORD,
        PIPE,
        REDIRECT_IN,
        REDIRECT_OUT,
        REDIRECT_APPEND,
        BACKGROUND,
        CD,
    };

    Token(Type type, std::string value) : type_(type), value_(value) {}
    Type GetType() const { return type_; }
    std::string GetValue() const { return value_; }

    std::string ToString() {
        switch (type_) {
        case Type::WORD:
            return "TOKEN_WORD: " + value_;
        case Type::PIPE:
            return "TOKEN_PIPE: " + value_;
        case Type::REDIRECT_IN:
            return "TOKEN_REDIRECT_IN: " + value_;
        case Type::REDIRECT_OUT:
            return "TOKEN_REDIRECT_OUT: " + value_;
        case Type::REDIRECT_APPEND:
            return "TOKEN_REDIRECT_APPEND: " + value_;
        case Type::BACKGROUND:
            return "TOKEN_BACKGROUND: " + value_;
        case Type::CD:
            return "TOKEN_CD: " + value_;
        }
    }

  private:
    Type type_;
    std::string value_;
};

class Lexer {
  public:
    void Lex(std::string line);
    std::vector<Token> tokens;
    bool HasBuiltins();
    void Clear();

  private:
    Token MatchToken(std::string token);
};
