#include "lexer.h"
#include <iostream>
#include <string>

void Lexer::Lex(std::string line) {
    std::string buffer;
    char c;

    for (int i = 0; i < line.length(); i++) {
        c = line[i];
        if (c == ' ') {
            Token token = MatchToken(buffer);
            if (token == Token::TOKEN_WORD) {
                words.push_back(buffer);
            }
            tokens.push_back(token);
            buffer = "";
        } else {
            buffer += c;
        }
    }
    // Push the last token
    Token token = MatchToken(buffer);
    tokens.push_back(token);
    if (token == Token::TOKEN_WORD) {
        words.push_back(buffer);
    }
}

 Token Lexer::MatchToken(std::string token) {
    if (token == "|") {
        return Token::TOKEN_PIPE;
    } else if (token == "<") {
        return Token::TOKEN_REDIRECT_IN;
    } else if (token == ">") {
        return Token::TOKEN_REDIRECT_OUT;
    } else if (token == ">>") {
        return Token::TOKEN_REDIRECT_APPEND;
    } else if (token == "&") {
        return Token::TOKEN_BACKGROUND;
    } else if (token == "cd") {
        return Token::TOKEN_CD;
    } else {
        return Token::TOKEN_WORD;
    }
}

bool Lexer::HasBuiltins() {
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i] == Token::TOKEN_CD) {
            return true;
        }
    }
    return false;
}

void Lexer::Clear() {
    tokens.clear();
    words.clear();
}

std::ostream &operator<<(std::ostream &os, Token token) {
    switch (token) {
    case Token::TOKEN_WORD:
        os << "WORD";
        break;
    case Token::TOKEN_PIPE:
        os << "PIPE";
        break;
    case Token::TOKEN_REDIRECT_IN:
        os << "REDIRECT_IN";
        break;
    case Token::TOKEN_REDIRECT_OUT:
        os << "REDIRECT_OUT";
        break;
    case Token::TOKEN_REDIRECT_APPEND:
        os << "REDIRECT_APPEND";
        break;
    case Token::TOKEN_BACKGROUND:
        os << "BACKGROUND";
        break;
    case Token::TOKEN_CD:
        os << "CD";
        break;
    }
    return os;
}
