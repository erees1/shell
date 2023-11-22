#pragma once

#include "lexer.h"
#include <iostream>
#include <string>
#include <vector>

class ICommand {
    // Interface for all simple commands
  public:
    virtual ~ICommand() {} // Virtual destructor
    virtual int AddArgument(std::string argument) = 0;
    virtual int Execute(int fdin, int fdout) = 0;
    virtual int number_of_arguments() = 0;
    virtual void set_input_file(std::string input_file) = 0;
    virtual bool has_input_file() = 0;
};

class SimpleCommand : public ICommand {
  public:
    int AddArgument(std::string argument);
    int Execute(int fdin, int fdout);
    int number_of_arguments() { return args_.size(); }
    void set_input_file(std::string input_file) { input_file_ = input_file; }
    bool has_input_file() { return input_file_.empty(); }

  private:
    char **args_pointer_array();
    char *filepath();
    std::vector<std::string> args_;
    std::string filepath_;
    std::string filename_;
    std::string input_file_;
    std::vector<char *> execvp_args_; // Store C-style strings
};

class CdCommand : public ICommand {

  public:
    int AddArgument(std::string argument);
    int Execute(int fdin, int fdout);
    int number_of_arguments() {
        if (argument_.empty()) {
            return 0;
        } else {
            return 1;
        }
    }
    void set_input_file(std::string input_file) { input_file_ = input_file; }
    bool has_input_file() { return input_file_.empty(); }

  private:
    std::string argument_;
    std::string input_file_;
};

class Command {
  public:
    int Parse(Lexer &lexer);
    int Execute();
    ~Command();

  private:
    std::vector<ICommand *> simple_commands_;
    std::string out_file_;
    std::string err_file_;
    int background_;
    void AddSimpleCommand(ICommand *simple_command);
};
