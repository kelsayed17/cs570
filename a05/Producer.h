//
// Created by Khaled Elsayed and Mustafa Hayeri.
//

#ifndef A05_PRODUCER_H
#define A05_PRODUCER_H

#include "Thread.h"
#include "Buffer.h"
#include "Item.h"

class Producer : public Thread {
public:
    explicit Producer(Buffer *buffer, producer_t producer, char const *name, int delay) :
            buffer(buffer), producer(producer), Thread(name), delay(delay) {}

    int getProducedCount() {
        return producedCount;
    };

protected:
    void *run() override;

private:
    Buffer *buffer;
    producer_t producer;
    int delay;
    int producedCount = 0;

    Item *produce();
};


#endif //A05_PRODUCER_H
