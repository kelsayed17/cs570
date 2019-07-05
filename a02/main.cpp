#include "tokenizer.hpp"
#include "command.hpp"
#include <string>
#include <list>
#include <readline/readline.h>
#include <iostream>

using namespace std;

// Main method that will continue to output the '>' character
// and will prompt for input. It will terminate on CTRL + D.
int main() {
    char *input;
    while ((input = readline(">")) != nullptr) {
        tokenizer tokenizer;
        list<string> *tokens = tokenizer.tokenize(input);
        command::execute_commands(tokens);
        delete tokens;
        free(input);
    }
    // This is to create a new line after the program has
    // terminated. 
    cout << endl;
    return 0;
}