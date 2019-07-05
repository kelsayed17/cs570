#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>
#include <list>

using namespace std;

class tokenizer {
private:
    static bool isSpecial(char c);

    static bool isDoubleQuote(char c);

    static bool isSingleQuote(char c);

    static bool isEscape(char c);

public:
    list<string> *tokenize(char *line);
};

#endif