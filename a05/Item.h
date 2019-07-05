//
// Created by Khaled Elsayed and Mustafa Hayeri.
//

#ifndef A05_ITEM_H
#define A05_ITEM_H

typedef enum {
    Frog,
    Escargot
} producer_t;

typedef enum {
    Lucy,
    Ethel
} consumer_t;

struct Item {
public:
    Item(producer_t item, char const *name) :
            item(item), name(name) {}

    ~Item() = default;

    const char *getName() {
        return name;
    }

    producer_t getValue() {
        return item;
    }

private:
    producer_t item;
    char const *name;
};

#endif //A05_ITEM_H
