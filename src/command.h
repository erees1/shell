#pragma once

#include "lexer.h"
#include <iostream>
#include <string>
#include <vector>

class SimpleCommand {
  public:
    char **args_pointer_array();
    char *filepath();
    void AddArgument(std::string argument);
    std::string input_file;

  private:
    int number_of_arguments_;
    std::vector<std::string> args_;
    std::string filepath_;
    std::string filename_;
    std::vector<char *> execvp_args_; // Store C-style strings
};

class Command {
  public:
    void parse(Lexer &lexer);
    int execute();
    ~Command();

  private:
    std::vector<SimpleCommand> simple_commands_;
    std::string out_file_;
    std::string input_file_;
    std::string err_file_;
    int background_;
    static Command current_command_;
    void AddSimpleCommand(SimpleCommand &simple_command);
};
