#include "command.h"
#include "loop.h"
#include <fcntl.h> // for open
#include <iostream>
#include <string>
#include <unistd.h>
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
    if (this->number_of_arguments_ == 0) {
        filepath_ = argument;
        filename_ = getFilenameFromPath(args_.back()); // Get the filename
        execvp_args_.push_back(&filename_[0]); // Add the command to c_args
    } else {
        // For subsequent arguments, just add their pointers to c_args
        execvp_args_.push_back(&args_.back()[0]);
    }
    this->number_of_arguments_++;
}

char **SimpleCommand::args_pointer_array() {
    // Add a nullptr to the end of c_args
    execvp_args_.push_back(nullptr);
    return execvp_args_.data();
}
char *SimpleCommand::filepath() {
    // Add a nullptr to the end of c_args
    return &filepath_[0];
}

Command::~Command(){
    // TODO this is causing a segfault
    // Delete all the SimpleCommands
    /* for (int i = 0; i < simple_commands_.size(); i++) { */
    /*     delete &simple_commands_[i]; */
    /* } */
};

void Command::AddSimpleCommand(SimpleCommand &simple_command) {
    simple_commands_.push_back(simple_command);
}

void Command::parse(Lexer &lexer) {
    std::vector<Token> tokens = lexer.tokens;
    std::vector<std::string> words = lexer.words;

    int wordIndex = 0;
    SimpleCommand *current_command = new SimpleCommand();
    for (int i = 0; i < tokens.size(); i++) {

        Token token = tokens[i];
        switch (token) {
        case Token::TOKEN_WORD: {
            current_command->AddArgument(words[wordIndex]);
            wordIndex++;
            break;
        }
        case Token::TOKEN_REDIRECT_IN: {
            // print word index
            input_file_ = words[wordIndex];
            current_command->input_file = input_file_;
            wordIndex++;
            i++; // Skip the next word
            break;
        }
        case Token::TOKEN_REDIRECT_OUT: {
            out_file_ = words[wordIndex];
            wordIndex++;
            i++; // Skip the next word
            break;
        }
        case Token::TOKEN_REDIRECT_APPEND: {
            out_file_ = words[wordIndex];
            wordIndex++;
            i++; // Skip the next word
            break;
        }
        case Token::TOKEN_BACKGROUND: {
            background_ = 1;
            break;
        }
        case Token::TOKEN_PIPE: {
            AddSimpleCommand(*current_command);
            current_command = new SimpleCommand();
            break;
        }
        default: {
            break;
        }
        } // switch
    }     // for
    AddSimpleCommand(*current_command);
};

int Command::execute() {
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
