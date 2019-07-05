//
// Created by Khaled Elsayed and Mustafa Hayeri.
//

#ifndef A05_BUFFER_H
#define A05_BUFFER_H

#include "Item.h"
#include <queue>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>

#define MAX_PRODUCTION 100
#define BUFFER_SIZE 10
#define FROG_LIMIT 3
#define SUCCESS 0
#define BARRIER 2
#define UNCONSUMED 0

using namespace std;

class Buffer {

public:
    Buffer() {
        if (pthread_mutex_init(&mutex, nullptr) != SUCCESS) {
            fprintf(stderr, "Unable to initialize mutex semaphore.\n");
            exit(EXIT_FAILURE);
        }

        if (sem_init(&unconsumed, 0, UNCONSUMED) != SUCCESS) {
            fprintf(stderr, "Unable to initialize unconsumed semaphore.\n");
            exit(EXIT_FAILURE);
        }

        if (sem_init(&availableSlots, 0, BUFFER_SIZE) != SUCCESS) {
            fprintf(stderr, "Unable to initialize available slots semaphore.\n");
            exit(EXIT_FAILURE);
        }

        if (sem_init(&availableFrogSlots, 0, FROG_LIMIT) != SUCCESS) {
            fprintf(stderr, "Unable to initialize available frog slots semaphore.\n");
            exit(EXIT_FAILURE);
        }

        if (sem_init(&availableProductionSlots, 0, MAX_PRODUCTION) != SUCCESS) {
            fprintf(stderr, "Unable to initialize available production slots semaphore.\n");
            exit(EXIT_FAILURE);
        }

        if (sem_init(&availableConsumptionSlots, 0, MAX_PRODUCTION) != SUCCESS) {
            fprintf(stderr, "Unable to initialize available consumption slots semaphore.\n");
            exit(EXIT_FAILURE);
        }

        if (pthread_barrier_init(&barrier, nullptr, BARRIER) != SUCCESS) {
            fprintf(stderr, "Unable to initialize barrier.\n");
            exit(EXIT_FAILURE);
        }
    }

    ~Buffer();

    Item *remove(char const *name);

    void add(Item *item);

    pthread_mutex_t mutex{};
    pthread_barrier_t barrier{};

    sem_t unconsumed{};
    sem_t availableSlots{};
    sem_t availableFrogSlots{};
    sem_t availableProductionSlots{}; // Keeps track of candies left to produce, starts at MAX_PRODUCTION
    sem_t availableConsumptionSlots{}; // Keeps track of candies left to consume, starts at MAX_PRODUCTION
    bool finished = false;

private:
    queue<Item *> buffer{};
    int beltFrogCount = 0;
    int beltEscargotCount = 0;
    int totalProducedCount = 0;
};


#endif //A05_BUFFER_H
