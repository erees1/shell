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
    int Parse(const std::vector<Token> &tokens);
    int Execute();
    ~CommandPipeline();

  private:
    std::vector<CommandInterface *> simple_commands_;
    std::string out_file_;
    bool append_out_ = false;
    std::string in_file_;
    std::string err_file_;
    bool background_ = false;
    void AddSimpleCommand(CommandInterface *simple_command);
};
