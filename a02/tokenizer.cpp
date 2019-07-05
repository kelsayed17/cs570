#include "tokenizer.hpp"
#include <string>
#include <list>

using namespace std;
////////////////////////////////////////////////////////////////////////////////

bool tokenizer::isSpecial(char c) {
    return c == '|' || c == ';' || c == '<' || c == '>' || c == '&';
}

bool tokenizer::isDoubleQuote(char c) {
    return c == '"';
}

bool tokenizer::isSingleQuote(char c) {
    return c == '\'';
}

bool tokenizer::isEscape(char c) {
    return c == '\\';
}

list<string> *tokenizer::tokenize(char *line) {
    // List of tokens to be collected
    auto *tokens = new list<string>();

    // The current token being constructed
    string token;

    // Conditional flags
    bool doubleQuote = false;
    bool singleQuote = false;
    bool escaped = false;

    // The current char in the input line
    char c;

    while ((c = *line++) != '\0') {
        // Encountered a space and other conditions were not triggered
        if (isspace(c) && !doubleQuote && !singleQuote && !escaped) {
            if (token.length() != 0) {
                tokens->push_back(token);
                token = "";
            }
        }
            // Encountered a special character and escape condition not triggered
        else if (isSpecial(c) && !escaped) {
            if (token.length() != 0) {
                tokens->push_back(token);
                token = "";
            }

            token.push_back(c); // Add special character to token
            tokens->push_back(token);
            token = "";
        }
            // Encountered an escape character
        else if (isEscape(c)) {
            escaped = !escaped;
        }
            // Encountered a double quote and other conditions were not triggered
        else if (isDoubleQuote(c) && !escaped && !singleQuote) {
            doubleQuote = !doubleQuote;
        }
            // Encountered a single quote and other conditions were not triggered
        else if (isSingleQuote(c) && !escaped && !doubleQuote) {
            singleQuote = !singleQuote;
        }
            // Escape condition was previously triggered
        else if (escaped) {
            // Escape character will be included in token
            if (isSingleQuote(c)) {
                token.push_back('\\'); // Add skipped escape character
                singleQuote = !singleQuote;
            } else {
                token.push_back(c);
            }
            escaped = !escaped;
        } else {
            token.push_back(c);
        }
    }

    // Add last token to list
    if (token.length() > 0) {
        tokens->push_back(token);
    }

    return tokens;
}