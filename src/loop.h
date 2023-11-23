#pragma once

#include "pipeline.h"
#include <string>

class Shell {
  public:
    void Loop();
    std::string ReadLine();
    int ParseAndExecute(CommandPipeline &command, Lexer &lexer);
};
