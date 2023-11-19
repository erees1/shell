#pragma once

#include <vector>
#include <string>

// Enum of tokens
enum class Token {
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_REDIRECT_APPEND,
    TOKEN_BACKGROUND,
};

std::ostream &operator<<(std::ostream &os, Token token);

class Lexer {
    public:
        void lex(std::string line); 
        std::vector<Token> tokens;
        std::vector<std::string> words;
    private:
        Token matchToken(std::string token);
};
