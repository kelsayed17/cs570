//
// Created by Khaled Elsayed and Mustafa Hayeri on 2019-03-02.
//

#include "level.h"
#include "PageTable.h"

level::level(int depth, level **nextLevelPtr) {
    this->depth = depth;
    this->nextLevelPtr = nextLevelPtr;
}

level::level(int depth, map *mapPtr) {
    this->depth = depth;
    this->mapPtr = mapPtr;
}

level::~level() = default;
