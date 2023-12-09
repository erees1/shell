#include "lexer.h"
#include <iostream>
#include <string>

std::vector<Token> Lexer::Lex(std::string line) {
    Clear();
    std::string buffer;
    char c;

    for (int i = 0; i < line.length(); i++) {
        c = line[i];
        if (c == ' ') {
            Token token = MatchToken(buffer);
            tokens_.push_back(token);
            buffer = "";
        } else {
            buffer += c;
        }
    }
    // Push the last token
    if (buffer != "") {
        Token token = MatchToken(buffer);
        tokens_.push_back(token);
    }
    return tokens_;
}

Token Lexer::MatchToken(std::string token) {
    if (token == "|") {
        return Token(Token::Type::PIPE, token);
    } else if (token == "<") {
        return Token(Token::Type::REDIRECT_IN, token);
    } else if (token == ">") {
        return Token(Token::Type::REDIRECT_OUT, token);
    } else if (token == ">>") {
        return Token(Token::Type::REDIRECT_APPEND, token);
    } else if (token == "&") {
        return Token(Token::Type::BACKGROUND, token);
    } else if (token == "cd") {
        return Token(Token::Type::CD, token);
    } else {
        return Token(Token::Type::WORD, token);
    }
}
