#include "command.h"
#include "loop.h"
#include <fcntl.h> // for open
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h> // for close
#include <vector>

std::string getFilenameFromPath(const std::string &path) {
    // Find the last occurrence of '/'
    size_t pos = path.find_last_of('/');

    if (pos == std::string::npos) {
        // No '/' found, the whole string is a filename
        return path;
    } else {
        // Return the substring after the last '/'
        return path.substr(pos + 1);
    }
}

int SimpleCommand::AddArgument(std::string argument) {
    args_.push_back(argument); // Add the argument to args vector
    return 0;
}

char **SimpleCommand::args_pointer_array() {
    for (int i = 0; i < args_.size(); i++) {
        if (i == 0) {
            filepath_ = args_[i];
            filename_ = getFilenameFromPath(filepath_);
            execvp_args_.push_back(&filename_[0]);
        } else {
            execvp_args_.push_back(&args_[i][0]);
        }
    }
    execvp_args_.push_back(nullptr);
    return execvp_args_.data();
};

char *SimpleCommand::filepath() { return &filepath_[0]; }

int SimpleCommand::Execute(int fdin, int fdout) {
    dup2(fdin, 0); // Set fdin to be stdin of the next command
    close(fdin);

    dup2(fdout, 1);
    close(fdout);

    int ret = fork();
    if (ret == 0) {
        // child
        execvp(this->filepath(), this->args_pointer_array());
        perror("execvp");
    } else if (ret < 0) {
        // parent
        perror("fork failed");
        _exit(1);
        ;
    }
    return ret;
};

void Command::AddSimpleCommand(ICommand *simple_command) {
    simple_commands_.push_back(simple_command);
}

int Command::Parse(Lexer &lexer) {
    std::vector<Token> tokens = lexer.tokens;

    int wordIndex = 0;
    ICommand *current_command = new SimpleCommand();
    for (int i = 0; i < tokens.size(); i++) {

        Token token = tokens[i];
        Token::Type token_type = token.GetType();
        switch (token_type) {
        case Token::Type::WORD: {
            current_command->AddArgument(token.GetValue());
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
            current_command = new SimpleCommand();
            break;
        }
        case Token::Type::CD: {
            current_command = new CdCommand();
            break;
        }
        } // switch
    }     // for
    if (current_command->number_of_arguments() > 0) {
        AddSimpleCommand(current_command);
    }
    return 0;
};

Command::~Command() {
    for (ICommand* cmd : simple_commands_) {
        delete cmd;
    }
}

int Command::Execute() {
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

        ICommand *simple_command = simple_commands_[i];

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

bool IsDir(std::string dir) {
    struct stat sb;
    if (stat(dir.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
        return true;
    }
    return false;
}

int CdCommand::AddArgument(std::string argument) {

    if (number_of_arguments() == 1) {
        std::cerr << "cd: expected either 0 or 1 argument, got "
                  << number_of_arguments() - 1 << std::endl;
        return 1;
    } else {
        // One argument, go to that directory
        if (IsDir(argument)) {
            argument_ = argument;
        } else {
            std::cerr << "cd: no such directory: " << argument << std::endl;
            return 1;
        }
    }
    return 0;
};

int CdCommand::Execute(int fdin, int fdout) {
    int ret = 0;
    if (argument_.empty()) {
        // No argument, go to home directory
        ret = chdir(getenv("HOME"));
    } else {
        ret = chdir(&argument_[0]);
    }
    return 0;
};
