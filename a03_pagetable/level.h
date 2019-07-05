//
// Created by Khaled Elsayed and Mustafa Hayeri on 2019-03-02.
//

#ifndef LEVEL_H
#define LEVEL_H

#include "map.h"

class PageTable;

class level {

public:
    int depth;
    level **nextLevelPtr = nullptr;
    map *mapPtr = nullptr;

    explicit level(int depth, level **nextLevelPtr);

    explicit level(int depth, map *mapPtr);

    ~level();
};

#endif //LEVEL_H
