#pragma once

#include "lexer.h"
#include "commands.h"
#include <iostream>
#include <string>
#include <vector>


class CommandPipeline {
    // Composite command handles a complete line entered into the shell
    // This may or may not contain pipes
  public:
    int Parse(Lexer &lexer);
    int Execute();
    ~CommandPipeline();

  private:
    std::vector<CommandInterface *> simple_commands_;
    std::string out_file_;
    std::string err_file_;
    int background_;
    void AddSimpleCommand(CommandInterface *simple_command);
};
