#pragma once

#include <string>
#include <vector>

class CommandInterface {
    // Interface for all simple commands
  public:
    virtual ~CommandInterface(){};
    virtual int AddArgument(std::string argument) = 0;
    virtual int Execute(int fdin, int fdout) = 0;
    virtual bool IsEmpty() = 0;
    virtual void set_input_file(std::string input_file) = 0;
    virtual bool has_input_file() = 0;
};

class ForkingCommand : public CommandInterface {
  public:
    int AddArgument(std::string argument);
    int Execute(int fdin, int fdout);
    bool IsEmpty() { return args_.size() == 0; }
    void set_input_file(std::string input_file) { input_file_ = input_file; }
    bool has_input_file() { return input_file_.empty(); }

  private:
    char **args_pointer_array();
    std::vector<std::string> args_;
    std::string filepath_;
    std::string filename_;
    std::string input_file_;
    std::vector<char *> execvp_args_; // Store C-style strings
};

class CdCommand : public CommandInterface {

  public:
    int AddArgument(std::string argument);
    int Execute(int fdin, int fdout);
    bool IsEmpty() { return false; }
    void set_input_file(std::string input_file) { input_file_ = input_file; }
    bool has_input_file() { return input_file_.empty(); }

  private:
    std::string argument_;
    std::string input_file_;
};