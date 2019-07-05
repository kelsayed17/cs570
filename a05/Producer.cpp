//
// Created by Khaled Elsayed and Mustafa Hayeri.
//

#include "Producer.h"
#include <thread>

void *Producer::run() {
    Item *item;
    while (true) {
        // Decrement available production slots (initial value = MAX_PRODUCTION)
        sem_wait(&buffer->availableProductionSlots);

        // Trap for sleeping threads that wake after first barrier is triggered
        if (buffer->finished)
            break;

        item = produce();

        // Decrement available frog slots
        if (item->getValue() == Frog) {
            sem_wait(&buffer->availableFrogSlots);
        }

        // Make sure we have room
        sem_wait(&buffer->availableSlots);

        // Access buffer exclusively
        pthread_mutex_lock(&buffer->mutex);
        buffer->add(item);
        pthread_mutex_unlock(&buffer->mutex);

        // Inform consumer
        sem_post(&buffer->unconsumed);

        // Sleep for specified time
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    return EXIT_SUCCESS;
}

Item *Producer::produce() {
    producedCount++;
    return new Item(producer, getName());
}
