//
// Created by Khaled Elsayed and Mustafa Hayeri on 2019-03-02.
//

#ifndef MAP_H
#define MAP_H

#include <cstdint>

struct map {
    uint32_t frame = 0;
    bool isValid = false;

    ~map() = default;
};

#endif //MAP_H
