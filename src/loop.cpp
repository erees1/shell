#include "loop.h"
#include "lexer.h"
#include "pipeline.h"
#include <iostream>

void Shell::Loop() {
    Lexer lexer;
    while (true) {
        std::cout << "shell> ";
        std::string line = ReadLine();
        lexer.Lex(line);
        CommandPipeline command_pipeline;
        ParseAndExecute(command_pipeline, lexer);
        lexer.Clear();
    }
}

int Shell::ParseAndExecute(CommandPipeline &command_pipeline, Lexer &lexer) {
    int ret = command_pipeline.Parse(lexer);
    if (ret != 0) {
        return ret;
    }
    ret = command_pipeline.Execute();
    return ret;
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
