//
// Created by Khaled Elsayed on 2019-04-13.
//

#include "Buffer.h"
#include <cstdio>

Item *Buffer::remove(char const *name) {
    Item *item = buffer.front();
    buffer.pop();

    switch (item->getValue()) {
        case Frog:
            beltFrogCount--;
            break;
        case Escargot:
            beltEscargotCount--;
            break;
    }

    // Display change
    printf("Belt: %d frogs + %d escargots = %lu. produced: %d\t%s consumed %s.\n",
           beltFrogCount, beltEscargotCount, buffer.size(), totalProducedCount, name,
           item->getName());
    fflush(stdout);

    return item;
}

void Buffer::add(Item *item) {
    buffer.push(item);

    switch (item->getValue()) {
        case Frog:
            beltFrogCount++;
            break;
        case Escargot:
            beltEscargotCount++;
            break;
    }

    totalProducedCount++;

    // Display change
    printf("Belt: %d frogs + %d escargots = %lu. produced: %d\tAdded %s.\n",
           beltFrogCount, beltEscargotCount, buffer.size(), totalProducedCount, item->getName());
    fflush(stdout);
}

Buffer::~Buffer() {
    sem_destroy(&unconsumed);
    sem_destroy(&availableSlots);
    sem_destroy(&availableFrogSlots);
    sem_destroy(&availableProductionSlots);
    sem_destroy(&availableConsumptionSlots);
}
