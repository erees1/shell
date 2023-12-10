#pragma once

#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

// Enum of tokens
class Token {

  public:
    enum class Type {
        // Shell redirect stuff
        PIPE,
        REDIRECT_IN,
        REDIRECT_OUT,
        REDIRECT_APPEND,
        BACKGROUND,

        // Arguments or Commands
        WORD,

        // Builtins
        CD,
    };

    Token(Type type, std::string value) : type_(type), value_(value) {}
    Type GetType() const { return type_; }
    std::string GetValue() const { return value_; }
    static std::unordered_map<std::string, Type> keywords;

  private:
    Type type_;
    std::string value_;
};

class Lexer {
  public:
    std::vector<Token> ScanTokens();
    Lexer(std::string line) : source_(line){};

  private:
    std::string source_;
    int start_ = 0;
    int current_ = 0;
    std::vector<Token> tokens_;

    void AddToken(Token::Type type);
    void AddToken(Token::Type type, std::string value);
    void ScanToken();

    bool IsAtEnd() { return current_ >= source_.length(); }
    char Advance(); // Consume and increment current_ character
    char Peek();    // Return the current_ character without incrementing
    bool Match(char expected); // Consume and increment current_ character if it
                               // matches

    void String(); // Match strings (i.e. "delimited")
    void Word();   // Match words (i.e. not delimted, break on spaces)
};
