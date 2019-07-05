#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <list>

using namespace std;

class command {
private:
    static void execute(int argc, char **argv);

    static void start(char **argv);

public:
    static void execute_commands(list<string> *tokens);
};

#endif