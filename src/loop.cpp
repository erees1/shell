#include "loop.h"
#include "command.h"
#include "lexer.h"
#include <iostream>

void Shell::Loop() {
    while (1) {
        std::cout << "shell> ";
        std::string line = ReadLine();
        Lexer lexer;
        lexer.Lex(line);
        if (lexer.HasBuiltins()) {
            BuiltinCommand builtin_command;
            ParseAndExecute(builtin_command, lexer);
        } else {
            Command command;
            ParseAndExecute(command, lexer);
        }
    }
}

int Shell::ParseAndExecute(ICommand &command, Lexer &lexer) {
    int ret = command.Parse(lexer);
    if (ret != 0) {
        return ret;
    }
    ret = command.Execute();
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

    return buffer;
}
