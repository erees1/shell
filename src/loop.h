#pragma once

#include "command.h"
#include <string>

class Shell {
  public:
    void Loop();
    std::string ReadLine();
    int ParseAndExecute(Command &command, Lexer &lexer);
};
