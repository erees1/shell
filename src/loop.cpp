#include "loop.h"
#include "command.h"
#include "lexer.h"
#include <iostream>

void Shell::Loop() {
    while (1) {
        std::cout << "shell> ";
        std::string line = ReadLine();
        Lexer lexer = Lexer();

        lexer.lex(line);

        Command command = Command();
        command.parse(lexer);
        command.execute();

        /* delete &command; */
        /* delete &lexer; */
    }
}

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
