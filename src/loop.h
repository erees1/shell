#pragma once

#include "pipeline.h"
#include <string>

class Shell {
  public:
    void Loop();
    static Shell *&GetInstance() {
        if (instance_ == nullptr) {
            instance_ = new Shell();
        }
        return instance_;
    };
    // Method to clean up the singleton instance
    static void cleanup() {
        if (instance_ != nullptr) {
            delete instance_;
            instance_ = nullptr;
        }
    }
    void SendSigintToForegroundProcessGroup() {
        if (command_pipeline_ != nullptr) {
            command_pipeline_->SendSignal(SIGINT);
        }
    }
    void SetExitFromRead() {
        std::cout << "Setting exit from read" << std::endl;
        should_exit_from_read_ = true;
    }

  private:
    static Shell *instance_;
    Shell() { lexer_ = new Lexer(); }
    ~Shell() {
        delete lexer_;
        if (command_pipeline_ != nullptr) {
            delete command_pipeline_;
        }
    }
    // Prevent copy-construction and assignment
    Shell(const Shell &) = delete;
    Shell &operator=(const Shell &) = delete;
    void RegisterSignalHandlers();
    Lexer *lexer_;
    std::string ReadLine();
    int ParseTokensAndExecute(const std::vector<Token> &tokens);
    CommandPipeline *command_pipeline_ = nullptr;
    bool should_exit_from_read_ = false;
};
