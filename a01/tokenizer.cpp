#include <string>
#include <iostream>
#include <cstdio>
#include <list>
#include <readline/readline.h>

using namespace std;

bool isSpecial(char c) {
    return c == '|' || c == ';' || c == '<' || c == '>' || c == '&';
}

bool isDoubleQuote(char c) {
    return c == '"';
}

bool isSingleQuote(char c) {
    return c == '\'';
}

bool isEscape(char c) {
    return c == '\\';
}

void print(list<string> tokens) {
    if (!tokens.empty()) {
        cout << "[";
        for (auto token = tokens.begin();
             token != tokens.end(); token++) {
            if (token != tokens.begin())
                cout << ",";
            cout << "{" + *token + "}";
        }
        cout << "]" << endl;
    }
}

int main(int argc, char *argv[]) {
    while (true) {
        char *line = readline(">");

        if (!line)
            break;

        string token;
        list<string> tokens;
        bool doubleQuote = false;
        bool singleQuote = false;
        bool escaped = false;
        char c;
        int index = 0;

        while ((c = line[index++]) != '\0') {
            // Encountered a space and other conditions were not triggered
            if (isspace(c) && !doubleQuote && !singleQuote && !escaped) {
                // Only if current token is not empty
                if (token.length() > 0) {
                    tokens.push_back(token); // Add token to list
                    token = ""; // Reset token
                }
            }
                // Encountered a special character and escape condition was not triggered
            else if (isSpecial(c) && !escaped) {
                // Only if current token is not empty
                if (token.length() > 0) {
                    tokens.push_back(token); // Add token to list
                    token = ""; // Reset token
                }

                token.push_back(c); // Add special character to token
                tokens.push_back(token); // Add token to list
                token = ""; // Reset token
            }
                // Encountered an escape character
            else if (isEscape(c)) {
                escaped = !escaped; // Flip the escape switch
            }
                // Encountered a double quote and other conditions were not triggered
            else if (isDoubleQuote(c) && !escaped && !singleQuote) {
                doubleQuote = !doubleQuote; // Flip the double quote switch
            }
                // Encountered a single quote and other conditions were not triggered
            else if (isSingleQuote(c) && !escaped && !doubleQuote) {
                singleQuote = !singleQuote; // Flip the single quote switch
            }
                // Escape condition was previously triggered
            else if (escaped) {
                // Escape character will be included in token, closing single quote will not
                if (isSingleQuote(c)) {
                    token.push_back(
                            '\\'); // Add previously skipped escape character
                    singleQuote = !singleQuote; // Flip the single quote switch
                } else {
                    token.push_back(
                            c); // Default behavior is to add character to token
                }
                escaped = !escaped; // Flip the escaped trigger to default state
            } else {
                token.push_back(
                        c); // Default behavior is to add character to token
            }
        }

        // Only if last token is not empty
        if (token.length() > 0)
            tokens.push_back(token); // Add last token to list

        print(tokens);
    }
    return 0;
}