#include "pipeline.h"
#include "loop.h"
#include <fcntl.h> // for open
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h> // for close
#include <vector>

void CommandPipeline::AddSimpleCommand(CommandInterface *simple_command) {
    simple_commands_.push_back(simple_command);
}

int CommandPipeline::Parse(Lexer &lexer) {
    std::vector<Token> tokens = lexer.tokens;

    int wordIndex = 0;
    CommandInterface *current_command = new ForkingCommand();
    for (int i = 0; i < tokens.size(); i++) {

        Token token = tokens[i];
        Token::Type token_type = token.GetType();
        switch (token_type) {
        case Token::Type::WORD: {
            int ret = current_command->AddArgument(token.GetValue());
            if (ret != 0) {
                return ret;
            }
            wordIndex++;
            break;
        }
        case Token::Type::REDIRECT_IN: {
            // print word index
            current_command->set_input_file(tokens[i + 1].GetValue());
            i++; // Skip the next word
            break;
        }
        case Token::Type::REDIRECT_OUT: {
            out_file_ = tokens[i + 1].GetValue();
            wordIndex++;
            i++; // Skip the next word
            break;
        }
        case Token::Type::REDIRECT_APPEND: {
            out_file_ = tokens[i + 1].GetValue();
            i++; // Skip the next word
            break;
        }
        case Token::Type::BACKGROUND: {
            background_ = 1;
            break;
        }
        case Token::Type::PIPE: {
            AddSimpleCommand(current_command);
            current_command = new ForkingCommand();
            break;
        }
        case Token::Type::CD: {
            current_command = new CdCommand();
            break;
        }
        } // switch
    }     // for
    if (!current_command->IsEmpty()) {
        AddSimpleCommand(current_command);
    }
    return 0;
};

CommandPipeline::~CommandPipeline() {
    for (CommandInterface *cmd : simple_commands_) {
        delete cmd;
    }
}

int CommandPipeline::Execute() {
    // Save stdin and stdout as we need to restore it later
    int tmpin = dup(0);
    int tmpout = dup(1);

    int fdin;      // File descriptor for input of the next command
    int next_fdin; // File descriptor for input of the next command
    int fdout;     // File descriptor for output of the next command
    int ret;       // Return value of fork()

    next_fdin = dup(tmpin);
    for (int i = 0; i < simple_commands_.size(); i++) {
        fdin = next_fdin;

        CommandInterface *simple_command = simple_commands_[i];

        if (i == simple_commands_.size() - 1) {
            // Last command
            if (!out_file_.empty()) {
                fdout = open(&out_file_[0], O_WRONLY | O_CREAT | O_TRUNC, 0666);
            } else {
                // Use default output
                fdout = dup(tmpout);
            }
        } else {
            // Not last command so setup pipes
            int fdpipe[2];
            pipe(fdpipe);
            next_fdin = fdpipe[0]; // Input for the next command
            fdout = fdpipe[1];     // Output for the current command
        }
        ret = simple_command->Execute(fdin, fdout);

    } // for

    dup2(tmpin, 0);
    dup2(tmpout, 1);
    close(tmpin);
    close(tmpout);

    if (background_ == 0) {
        // Wait for last process to finish
        waitpid(ret, NULL, 0);
    }

    return 0;
} // execute
