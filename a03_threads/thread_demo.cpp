#include "worker.hpp"
#include <cstdio>
#include <pthread.h>
#include <sched.h>
#include <iostream>

using namespace std;

// How many threads to create
#define THREAD_NUM 5

int main() {
    pthread_t thread[THREAD_NUM];
    int return_code;

    // The tid is the thread id of the current spawned thread
    int tid;

    // Create a loop that spawns 5 threads with integer
    // arguments 0 through 4.
    for (tid = 0; tid < THREAD_NUM; tid++) {
        // Starting a new thread
        return_code = pthread_create(&thread[tid], nullptr, worker,
                                     (void *) (size_t) &tid);
        // Check for failed threads
        if (return_code) {
            cout << "ERROR: Thread was not created" << endl;
            exit(EXIT_FAILURE);
        }
        pthread_join(thread[tid], nullptr);
    }

    cout << "work complete" << endl;
    pthread_exit(nullptr);
}
