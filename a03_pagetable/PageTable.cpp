//
// Created by Khaled Elsayed and Mustafa Hayeri on 2019-03-02.
//

#include "PageTable.h"
#include "level.h"
#include <cmath>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

PageTable::PageTable(int levelCount, vector<int> levels) {
    uint32_t shift = 0;
    this->levelCount = levelCount;
    levelInfo = new PageTableInfo[levelCount]();
    for (int i = 0; i < levelCount; i++) {
        levelInfo[i].shift = 32 - (shift += levels[i]);
        levelInfo[i].entries = (int) pow(2, levels[i]);
        levelInfo[i].mask =
                (uint32_t) (levelInfo[i].entries - 1) << levelInfo[i].shift;
    }
    pageTableSize = (int) (pow(2, 32) / pow(2, shift));
    int entries = levelInfo[0].entries;
    if (levelCount == 1) {
        rootNodePtr = new level(0, new map[entries]());
        totalMemory += sizeof(map) * entries;
    } else {
        rootNodePtr = new level(0, new level *[entries]());
        totalMemory += sizeof(level) * entries;
    }
    totalMemory += sizeof(level);
    totalMemory += sizeof(PageTable);
}

uint32_t PageTable::logicalToPage(uint32_t logicalAddress, uint32_t mask,
                                  uint32_t shift) {
    return (logicalAddress & mask) >> shift;
}

bool PageTable::pageLookup(uint32_t logicalAddress, uint32_t &frame) {
    level *levelPtr = rootNodePtr;
    for (int i = 0; i < levelCount; i++) {
        if (levelPtr == nullptr)
            return false;
        uint32_t page = logicalToPage(logicalAddress, levelInfo[i].mask,
                                      levelInfo[i].shift);
        if (levelPtr->mapPtr) {
            frame = levelPtr->mapPtr[page].frame;
            return true;
        }
        levelPtr = levelPtr->nextLevelPtr[page];
    }
    return false;
}

void PageTable::pageToFrame(FILE *dumpFile, uint32_t page) {
    level *levelPtr = rootNodePtr;
    if (levelPtr->nextLevelPtr && levelPtr->nextLevelPtr[page]) {
        pageToFrame(dumpFile, levelPtr->nextLevelPtr[page], page);
    } else if (levelPtr->mapPtr && levelPtr->mapPtr[page].isValid) {
        uint32_t frame = levelPtr->mapPtr[page].frame;
        fprintf(dumpFile, "%08x -> %08x\n", page, frame);
    }
}

void PageTable::pageToFrame(FILE *dumpFile, level *levelPtr, uint32_t page) {
    int depth = levelPtr->depth;
    int entries = levelInfo[depth].entries;
    int bits = (int) (log(entries) / log(2));

    for (int i = 0; i < levelInfo[depth].entries; i++) {
        uint32_t shift = (page << bits) + i;
        if (levelPtr->mapPtr && levelPtr->mapPtr[i].isValid) {
            uint32_t frame = levelPtr->mapPtr[i].frame;
            fprintf(dumpFile, "%08x -> %08x\n", shift, frame);
        } else if (levelPtr->nextLevelPtr && levelPtr->nextLevelPtr[i]) {
            pageToFrame(dumpFile, levelPtr->nextLevelPtr[i], shift);
        }
    }
}

bool PageTable::logicalToPhysical(uint32_t logicalAddress,
                                  uint32_t &physicalAddress) {
    uint32_t frame;
    if (pageLookup(logicalAddress, frame)) {
        int offsetBits = (int) (log(pageTableSize) /
                                log(2)); // log2(totalPages)
        uint32_t mask = (uint32_t) pow(2, offsetBits) - 1;
        physicalAddress = (logicalAddress & mask) + (frame << offsetBits);
        return true;
    }
    return false;
}

void PageTable::pageInsert(uint32_t logicalAddress) {
    pageInsert(rootNodePtr, logicalAddress);
}

void PageTable::pageInsert(level *levelPtr, uint32_t logicalAddress) {
    int depth = levelPtr->depth;
    uint32_t page = logicalToPage(logicalAddress, levelInfo[depth].mask,
                                  levelInfo[depth].shift);

    // Leaf node
    if (depth == levelCount - 1) {
        if (levelPtr->mapPtr[page].isValid)
            hits++;
        else {
            levelPtr->mapPtr[page].frame = currentFrame++;
            levelPtr->mapPtr[page].isValid = true;
            misses++;
        }
        return;
    } // Inner node
    else if (levelPtr->nextLevelPtr[page] == nullptr) {
        int nextEntries = levelInfo[depth + 1].entries;
        if (depth == levelCount - 2) {
            levelPtr->nextLevelPtr[page] = new level(depth + 1,
                                                     new map[nextEntries]());
            totalMemory += sizeof(map) * nextEntries;
        } else {
            levelPtr->nextLevelPtr[page] = new level(depth + 1,
                                                     new level *[nextEntries]());
            totalMemory += sizeof(level) * nextEntries;
        }
        totalMemory += sizeof(level);
    }
    pageInsert(levelPtr->nextLevelPtr[page], logicalAddress);
}

void PageTable::displayStats() {
    int totalAddresses = hits + misses;
    double hitRate = ((double) hits / totalAddresses) * 100;
    double missRate = ((double) misses / totalAddresses) * 100;
    printf("Page table size: %d\n", pageTableSize);
    printf("Hits %d (%.2f%%), Misses %d (%.2f%%) # Addresses %d\n",
           hits, hitRate, misses, missRate, totalAddresses);
    printf("Bytes used: %d\n", totalMemory);
}

void PageTable::deletePages(level *levelPtr) {
    int depth = levelPtr->depth;
    if (depth == levelCount - 1) {
        delete[] levelPtr->mapPtr;
    } else if (levelPtr->nextLevelPtr) {
        for (int i = 0; i < levelInfo[depth].entries; i++) {
            if (levelPtr->nextLevelPtr[i]) {
                deletePages(levelPtr->nextLevelPtr[i]);
                delete levelPtr->nextLevelPtr[i];
            }
        }
        delete[] levelPtr->nextLevelPtr;
    }
}

PageTable::~PageTable() {
    deletePages(rootNodePtr);
    delete rootNodePtr;
}
