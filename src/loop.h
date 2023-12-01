#pragma once

#include "pipeline.h"
#include <string>

class Shell {
  public:
    void Loop();
    Shell() { lexer_ = new Lexer(); }
    ~Shell() { delete lexer_; }

  private:
    Lexer *lexer_;
    std::string ReadLine();
    int ParseTokensAndExecute(const std::vector<Token> &tokens);
};
