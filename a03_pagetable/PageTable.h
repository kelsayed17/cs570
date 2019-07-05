//
// Created by Khaled Elsayed and Mustafa Hayeri on 2019-03-02.
//

#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <cstdint>
#include <cstdio>
#include <vector>

using namespace std;

struct PageTableInfo {
    uint32_t mask;
    uint32_t shift;
    int entries;
};

class level;

class PageTable {
public:
    PageTableInfo *levelInfo;

    explicit PageTable(int level_count, vector<int> levels);

    ~PageTable();

    static uint32_t
    logicalToPage(uint32_t logical_address, uint32_t mask, uint32_t shift);

    bool logicalToPhysical(uint32_t logicalAddress, uint32_t &physicalAddress);

    bool pageLookup(uint32_t logicalAddress, uint32_t &frame);

    void pageInsert(uint32_t logicalAddress);

    void displayStats();

    void pageToFrame(FILE *dumpFile, uint32_t page);

    void pageToFrame(FILE *dumpFile, level *levelPtr, uint32_t page);

private:
    int pageTableSize = 0;
    uint32_t currentFrame = 0;
    level *rootNodePtr;
    int levelCount = 0;

    int hits = 0;
    int misses = 0;
    int totalMemory = 0;

    void deletePages(level *levelPtr);

    void pageInsert(level *levelPtr, uint32_t logicalAddress);
};


#endif //PAGETABLE_H
