#include "worker.hpp"
#include <cstdio>
#include <pthread.h>
#include <sched.h>
#include <iostream>

using namespace std;

void *worker(void *arg) {
    int argument = *(int *) arg;
    int argument_squared = argument * argument;
    cout << argument_squared << endl;
    pthread_exit(nullptr);
}
