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

void SimpleCommand::AddArgument(std::string argument) {
    args_.push_back(argument); // Add the argument to args vector
    // For the first argument (command), set filepath and add to c_args
    this->number_of_arguments++;
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

std::ostream &operator<<(std::ostream &os, SimpleCommand simple_command) {
    char **args = simple_command.args_pointer_array();
    for (int i = 0; args[i] != nullptr; i++) {
        std::cout << args[i] << " ";
    }
    return os;
}

void Command::AddSimpleCommand(SimpleCommand &simple_command) {
    simple_commands_.push_back(simple_command);
}

int Command::Parse(Lexer &lexer) {
    std::vector<Token> tokens = lexer.tokens;

    int wordIndex = 0;
    SimpleCommand current_command = SimpleCommand();
    for (int i = 0; i < tokens.size(); i++) {

        Token token = tokens[i];
        Token::Type token_type = token.GetType();
        switch (token_type) {
        case Token::Type::WORD: {
            current_command.AddArgument(token.GetValue());
            wordIndex++;
            break;
        }
        case Token::Type::REDIRECT_IN: {
            // print word index
            input_file_ = tokens[i + 1].GetValue();
            current_command.input_file = input_file_;
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
            current_command = SimpleCommand();
            break;
        }
        default: {
            break;
        }
        } // switch
    }     // for
    if (current_command.number_of_arguments > 0) {
        AddSimpleCommand(current_command);
    }
    return 0;
};

int Command::Execute() {
    // This is the meat of the shell
    int tmpin = dup(0);
    int tmpout = dup(1);
    int fdin;
    int fdout;
    int ret;

    fdin = dup(tmpin);
    for (int i = 0; i < simple_commands_.size(); i++) {
        SimpleCommand simple_command = simple_commands_[i];

        if (i == 0) {
            // First command
            if (!input_file_.empty()) {
                fdin = open(&input_file_[0], O_RDONLY);
            }
        }
        dup2(fdin, 0);
        close(fdin);

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
            fdin = fdpipe[0];
            fdout = fdpipe[1];
        }

        dup2(fdout, 1);
        close(fdout);

        ret = fork();
        if (ret == 0) {
            // child
            execvp(simple_command.filepath(),
                   simple_command.args_pointer_array());
            perror("execvp");
        } else if (ret < 0) {
            // parent
            perror("fork failed");
            _exit(1);
            ;
        }
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

int BuiltinCommand::Parse(Lexer &lexer) {
    if (lexer.tokens[0].GetType() == Token::Type::CD) {
        if (lexer.tokens.size() > 2) {
            std::cerr << "cd: expected either 0 or 1 argument, got "
                      << lexer.tokens.size() - 1 << std::endl;
            return 1;
        } else if (lexer.tokens.size() == 2) {
            // One argument, go to that directory
            std::string arg = lexer.tokens[1].GetValue();
            if (IsDir(arg)) {
                arguments_.push_back(arg);
            } else {
                std::cerr << "cd: no such directory: " << arg << std::endl;
                return 1;
            }
        }
        command_ = Builtins::TOKEN_CD;
    };
    return 0;
};
int BuiltinCommand::Execute() {
    int ret = 0;
    if (command_ == Builtins::TOKEN_CD) {
        if (arguments_.size() == 0) {
            // No argument, go to home directory
            ret = chdir(getenv("HOME"));
        } else {
            ret = chdir(&arguments_.back()[0]);
        }
    };
    return ret;
};
