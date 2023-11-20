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
    int number_of_arguments;

  private:
    std::vector<std::string> args_;
    std::string filepath_;
    std::string filename_;
    std::vector<char *> execvp_args_; // Store C-style strings
};

class ICommand {
    // Interface for all commands
  public:
    virtual int Parse(Lexer &lexer) = 0;
    virtual int Execute() = 0;
};

enum class Builtins {
    TOKEN_CD,
};

class BuiltinCommand : public ICommand {

  public:
    int Parse(Lexer &lexer);
    int Execute();

  private:
    Builtins command_;
    std::vector<std::string> arguments_;
};

class Command : public ICommand {
  public:
    int Parse(Lexer &lexer);
    int Execute();

  private:
    std::vector<SimpleCommand> simple_commands_;
    std::string out_file_;
    std::string input_file_;
    std::string err_file_;
    int background_;
    static Command current_command_;
    void AddSimpleCommand(SimpleCommand &simple_command);
};
