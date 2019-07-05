#include "command.hpp"
#include <string>
#include <list>
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>

using namespace std;

// Pass in our list of tokens to be grouped into an arguments
void command::execute_commands(list<string> *tokens) {
    // The limit placed on the number of arguments that can be passed
    const int MAX_ARGS = 100;
    char **argv = new char *[MAX_ARGS + 1]; // Arguments
    int argc = 0; // Number of arguments
    bool ignore_char = false; // Ignore flag for text that follows a redirect


    auto token_iterator = tokens->begin();
    while (token_iterator != tokens->end()) {
        // The value of the current token pointer
        string token = *token_iterator;

        if (token == ";" || token == "|") {
            if (token == "|")
                cout << "Pipe not implemented." << endl;

            argv[argc] = nullptr; // Last argument must be NULL
            execute(argc, argv); // Execute current args

            // Reset
            ignore_char = false;
            argc = 0;
        } else if (token == "<" || token == ">" || token == "&") {
            cout << "IO redirection and background not implemented." << endl;

            // Text after redirect is ignored
            if (token == "<" || token == ">")
                ignore_char = true;
        } else if (!ignore_char) {
            // If the number of arguments equals the limit of arguments
            // then an error is printed
            if (argc == MAX_ARGS) {
                cout << "Error: too many arguments" << endl;
                delete[] argv;
                return;
            }

            // Start of a new argument
            char *arg = new char[token.length() + 1];
            strcpy(arg, token.c_str());
            argv[argc++] = arg;
        }
        token_iterator++;
    }

    // Null terminate last argument and execute current args
    argv[argc] = nullptr;
    execute(argc, argv);
    delete[] argv;
}

// Determines whether to change directory, print working directory,
// or start a process. 
void command::execute(int argc, char **argv) {
    if (argc == 0)
        return;

    // Checks if the first argument is a program name 
    const char *program = argv[0];

    if (strcmp(program, "cd") == 0) {
        if (argc != 2) {
            cout << "Accepts exactly one argument." << endl;
        } else {
            // Return of anything other than 0 indicates error
            if (chdir(argv[1]) != 0)
                cout << "Directory does not exist or is not accessible."
                     << endl;
        }
    } else if (strcmp(program, "pwd") == 0) {
        char cwd[FILENAME_MAX];
        if (argc != 1)
            cout << "Accepts no arguments." << endl;
        else if (getcwd(cwd, sizeof(cwd)) == nullptr)
            cout << "Unable to obtain current directory" << endl;
        else
            cout << cwd << endl;
    }
        // The list of arguments is a process that needs to run
    else {
        start(argv);
    }
}

// Method starts a process by attempting to fork the 
// parent process
void command::start(char **argv) {
    pid_t pid;
    int status;

    if ((pid = fork()) < 0) {
        cout << "Unable to spawn program." << endl;
    }
        // We are in the child process
    else if (pid == 0) {
        // Child process could not start, exit normally
        if (execvp(argv[0], argv) < 0) {
            printf("Unable to execute %s\n", argv[0]);
            exit(EXIT_SUCCESS);
        }
    }
        // We are in the parent process
    else {
        // Wait for child process to return status
        while (wait(&status) != pid);
        if (status == 0)
            cout << "Process exited successfully" << endl;
        else
            cout << "Process exited with error" << endl;
    }
}