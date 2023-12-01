#include "pipeline.h"
#include "loop.h"
#include <csignal>
#include <fcntl.h> // for open
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h> // for close
#include <vector>

void CommandPipeline::AddSimpleCommand(CommandInterface *simple_command) {
    simple_commands_.push_back(simple_command);
}

int CheckToken(const std::vector<Token> &tokens, int i) {
    if (i >= tokens.size()) {
        std::cerr << "syntax error: missing argument after redirect"
                  << std::endl;
        return -1; // or some other error code
    }
    return 1;
}

int CommandPipeline::Parse(const std::vector<Token> &tokens) {

    CommandInterface *current_command = nullptr;
    for (int i = 0; i < tokens.size(); i++) {

        Token token = tokens[i];
        Token::Type token_type = token.GetType();

        switch (token_type) {
        case Token::Type::WORD: {
            if (current_command == nullptr) {
                current_command = new ForkingCommand();
                AddSimpleCommand(current_command);
            }
            int ret = current_command->AddArgument(token.GetValue());
            if (ret != 0) {
                return ret;
            }
            break;
        }
        case Token::Type::REDIRECT_IN: {
            // print word index
            if (CheckToken(tokens, i + 1) == -1) {
                return -1;
            }
            in_file_ = tokens[i + 1].GetValue();
            // only allow this for the first command
            if (simple_commands_.size() > 1) {
                std::cerr << "ambiguous input redirect" << std::endl;
                return -1;
            }
            i++; // Skip the next word
            break;
        }
        case Token::Type::REDIRECT_OUT: {
            if (CheckToken(tokens, i + 1) == -1) {
                return -1;
            }
            out_file_ = tokens[i + 1].GetValue();
            i++; // Skip the next word
            break;
        }
        case Token::Type::REDIRECT_APPEND: {
            if (CheckToken(tokens, i + 1) == -1) {
                return -1;
            }
            out_file_ = tokens[i + 1].GetValue();
            append_out_ = true;
            i++; // Skip the next word
            break;
        }
        case Token::Type::BACKGROUND: {
            background_ = true;
            break;
        }
        case Token::Type::PIPE: {
            if (current_command == nullptr) {
                std::cerr << "invalid syntax" << std::endl;
                return -1;
            }
            current_command = nullptr;
            break;
        }
        case Token::Type::CD: {
            if (current_command != nullptr) {
                std::cerr << "invalid syntax" << std::endl;
                return -1;
            }
            current_command = new CdCommand();
            AddSimpleCommand(current_command);
            break;
        }
        } // switch

    } // for
    return 0;
};

CommandPipeline::~CommandPipeline() {
    for (CommandInterface *cmd : simple_commands_) {
        delete cmd;
    }
}

int CommandPipeline::Execute() {
    int fdin = 0;      // File descriptor for input of the next command
    int next_fdin = 0; // File descriptor for input of the next command
    int fdout = 1;     // File descriptor for output of the next command

    for (int i = 0; i < simple_commands_.size(); i++) {
        fdin = next_fdin;
        CommandInterface *simple_command = simple_commands_[i];

        if (i == simple_commands_.size() - 1) {
            // Last command so setup output file if specified
            if (!out_file_.empty()) {
                if (append_out_) {
                    fdout = open(&out_file_[0], O_WRONLY | O_CREAT | O_APPEND,
                                 0666);
                } else {
                    fdout =
                        open(&out_file_[0], O_WRONLY | O_CREAT | O_TRUNC, 0666);
                }
            } else {
                fdout = 1;
            }
        } else {
            // Not last command so setup pipes
            int fdpipe[2];
            pipe(fdpipe);
            next_fdin = fdpipe[0]; // Input for the next command
            fdout = fdpipe[1];     // Output for the current command
        }

        if (i == 0 && !in_file_.empty()) {
            fdin = open(&in_file_[0], O_RDONLY);
        }
        int ret = simple_command->Execute(fdin, fdout);
        if (ret != 0) {
            pids_.push_back(ret);
        }

    } // for

    if (!background_) {
        WaitOnChildren();
    }
    pids_.clear();

    return 0;
} // execute

void CommandPipeline::SendSignal(int signal) {
    if (pids_.empty()) {
        return;
    }
    int pid = pids_.back();
    kill(pid, signal);
    // I am not sure why but I need to wait here otherwise the child becomes a
    // zombie despite waiting in execute when I ctrl-c
    WaitOnChildren();
}

std::vector<int> CommandPipeline::WaitOnChildren() {
    std::vector<int> ret_values;
    for (int pid : pids_) {
        int ret_code;
        waitpid(pid, &ret_code, 0);
        ret_values.push_back(ret_code);
    }
    pids_.clear();
    return ret_values;
}
