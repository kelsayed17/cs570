//
// Created by Khaled Elsayed and Mustafa Hayeri.
//

#ifndef A05_CONSUMER_H
#define A05_CONSUMER_H

#include "Thread.h"
#include "Buffer.h"

class Consumer : public Thread {
public:
    explicit Consumer(Buffer *buffer, consumer_t consumer, char const *name, int delay) :
            buffer(buffer), Thread(name), delay(delay) {}

    int getConsumedFrogCount() {
        return consumedFrogCount;
    };

    int getConsumedEscargotCount() {
        return consumedEscargotCount;
    };

protected:
    void *run() override;

private:
    Buffer *buffer;
    int delay;
    int consumedFrogCount = 0;
    int consumedEscargotCount = 0;

    void consume(Item *item);
};


#endif //A05_CONSUMER_H
