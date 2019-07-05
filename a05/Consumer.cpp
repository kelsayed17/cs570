//
// Created by Khaled Elsayed and Mustafa Hayeri.
//

#include "Consumer.h"
#include <thread>

#define STOP_VALUE 0

void *Consumer::run() {
    Item *item;
    int remainingItems;
    while (true) {
        // Block until something to consume
        sem_wait(&buffer->unconsumed);

        // Trap for sleeping threads that wake after first barrier is triggered
        if (buffer->finished)
            break;

        // Access buffer exclusively
        pthread_mutex_lock(&buffer->mutex);
        item = buffer->remove(getName());
        pthread_mutex_unlock(&buffer->mutex);

        // Increment available slots
        sem_post(&buffer->availableSlots);

        // Increment available frog slots
        if (item->getValue() == Frog) {
            sem_post(&buffer->availableFrogSlots);
        }

        consume(item);

        // Decrement available consumption slots (initial value = MAX_PRODUCTION)
        sem_wait(&buffer->availableConsumptionSlots);

        // Trap for sleeping threads that wake after first barrier is triggered
        if (buffer->finished)
            break;

        // Check if last item was consumed
        sem_getvalue(&buffer->availableConsumptionSlots, &remainingItems);
        if (remainingItems == STOP_VALUE) {
            buffer->finished = true;

            // Release sleeping threads: there are two possible locations where consumer thread could be sleeping
            sem_post(&buffer->unconsumed);
            sem_post(&buffer->availableConsumptionSlots);

            // Release sleeping threads: have to post twice to release because there are two producers
            sem_post(&buffer->availableProductionSlots);
            sem_post(&buffer->availableProductionSlots);

            // Finally we notify the barrier
            pthread_barrier_wait(&buffer->barrier);
            break;
        }

        // Sleep for specified time
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    return EXIT_SUCCESS;
}

void Consumer::consume(Item *item) {
    switch (item->getValue()) {
        case Frog:
            consumedFrogCount++;
            break;
        case Escargot:
            consumedEscargotCount++;
            break;
    }
    delete item;
}
