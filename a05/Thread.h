//
// Created by Khaled Elsayed and Mustafa Hayeri.
//

#ifndef A05_THREAD_H
#define A05_THREAD_H

#include "pthread.h"

class Thread {
public:
    explicit Thread(char const *name) : _name(name) {}

    virtual ~Thread() = default;

    int start() {
        return pthread_create(&_thread, nullptr, run, this);
    }

    int join() {
        return pthread_join(_thread, nullptr);
    }

    char const *getName() {
        return _name;
    }

    void setName(char const *name) {
        _name = name;
    }

protected:
    virtual void *run() = 0;

private:
    static void *run(void *arg) {
        return ((Thread *) arg)->run();
    }

    pthread_t _thread{};
    char const *_name{};
};


#endif //A05_THREAD_H
