#include "loop.h"
#include "lexer.h"
#include "pipeline.h"
#include <iostream>

void Shell::Loop() {
    while (true) {
        std::cout << "shell> ";
        std::string line = ReadLine();
        lexer_->Lex(line);
        ParseTokensAndExecute(lexer_->tokens);
        lexer_->Clear();
    }
}

int Shell::ParseTokensAndExecute(const std::vector<Token> &tokens) {
    CommandPipeline command_pipeline;
    int ret = command_pipeline.Parse(tokens);
    if (ret != 0) {
        return ret;
    }
    return command_pipeline.Execute();
};

std::string Shell::ReadLine() {
    std::string buffer;
    char c;

    while (true) {
        c = static_cast<char>(std::cin.get());

        // If we hit EOF or newline, return the string we've built so far.
        if (c == EOF || c == '\n') {
            break;
        } else {
            buffer += c;
        }
    }
    // Handle the case where the user hits ctrl-d to exit the shell.
    if (c == EOF && buffer.empty()) {
        std::cout << std::endl;
        exit(0);
    }
    return buffer;
}
