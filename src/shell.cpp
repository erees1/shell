#include "shell.h"
#include "lexer.h"
#include "pipeline.h"
#include <csignal>
#include <iostream>
#include <unistd.h>

// Shell is a singleton class
Shell *Shell::instance_ = nullptr;

void Error(std::string msg) { std::cerr << msg << std::endl; }

void SigintHandler(int sig) {
    std::cout << std::endl;
    Shell *shell = Shell::GetInstance();
    shell->SendSigintToForegroundProcessGroup();
    shell->SetExitFromRead();
}

void Shell::Loop() {
    RegisterSignalHandlers();
    while (true) {
        // sleep for 1 second
        std::cout << "shell> ";
        Lexer lexer = Lexer(Readline());
        std::vector<Token> tokens = lexer.ScanTokens();
        ParseTokensAndExecute(tokens);
    }
}

void Shell::RegisterSignalHandlers() {
    // Register signal hander for SIGINT (ctrl-c).
    struct sigaction sigint_action;
    sigint_action.sa_handler = SigintHandler;
    sigemptyset(&sigint_action.sa_mask);
    sigint_action.sa_flags = 0;
    sigaction(SIGINT, &sigint_action, NULL);
}

int Shell::ParseTokensAndExecute(const std::vector<Token> &tokens) {
    if (tokens.size() == 0) {
        // Nothing to do
        return 0;
    }
    if (command_pipeline_ != nullptr) {
        delete command_pipeline_;
    }
    command_pipeline_ = new CommandPipeline();
    int ret = command_pipeline_->Parse(tokens);
    if (ret != 0) {
        return ret;
    }
    return command_pipeline_->Execute();
};

std::string Shell::Readline() {
    std::string buffer;
    char c;

    should_exit_from_read_ = false;
    while (true) {
        c = static_cast<char>(std::cin.get());

        if (should_exit_from_read_) {
            should_exit_from_read_ = false;
            buffer.clear();
            std::cin.clear();
            break;
        } else if (std::cin.eof() &&
                   std::cin.eof()) { // Check for EOF explicitly
            std::cin.clear();
            exit(0);
        } else if (c == '\n') {
            break;
        } else {
            buffer += c;
        }
    }
    return buffer;
}
