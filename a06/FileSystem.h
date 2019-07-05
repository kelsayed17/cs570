//
// Created by Khaled Elsayed on 2019-05-05.
//

#ifndef DISKSIM_FILESYSTEM_H
#define DISKSIM_FILESYSTEM_H
#define UINT_BITS 32

#include <stdio.h>
#include <string>
#include <iostream>
#include <cmath>
#include <sstream>
#include "BlockDevice.h"
#include "SuperBlock.h"
#include <map>
#include <list>
#include <vector>
#include "Expression.h"
#include <bitset>

#define FILE_EXISTS 0
#define BLOCK_SIZE 4096

using namespace std;

typedef enum {
    FORMAT,
    PLACE,
    LIST,
    LISTBLOCKS,
    MKDIR,
    REMOVE,
    DUMP
} command_t;

typedef enum {
    SUCCESS,
    FAILURE

} result_t;

class FileSystem {
public:
    uint32_t *free_map{};
    int *fat_table{};
    int free_map_size = 0;

    explicit FileSystem(BlockDevice *blockDevice) : blockDevice(blockDevice) {
        commands["format"] = FORMAT;
        commands["place"] = PLACE;
        commands["list"] = LIST;
        commands["listblocks"] = LISTBLOCKS;
        commands["mkdir"] = MKDIR;
        commands["remove"] = REMOVE;
        commands["dump"] = DUMP;
    };

    result_t interpret(const string &line) {
        vector<string> tokens;
        string token;
        stringstream tokenStream(line);
        while (getline(tokenStream, token, ' ')) {
            tokens.push_back(token);
        }

        string command = tokens[0];

        switch (commands[command]) {
            case FORMAT:
                return format();
            case PLACE:
                return place(tokens[1], tokens[2]);
            case LIST:
                break;
            case LISTBLOCKS:
                break;
            case MKDIR:
                break;
            case REMOVE:
                break;
            case DUMP:
                break;
            default:

                break;
        }

        return FAILURE;
    }

    result_t format();

    result_t place(const string &src_file, const string &dest_path);

    result_t list();

    result_t listblocks();

    result_t mkdir(const string &path);

    result_t remove();

    result_t dump();

    BlockDevice *blockDevice;

private:


    void initialize(int start, int stop, int max) {
        for (int i = start; i < stop; i++) {
            int currentBlock = nextFreeBlock(i, max);
            toggleMapBit(currentBlock);
            if (i < stop - 1)
                updateFAT(currentBlock, nextFreeBlock(currentBlock, max));
        }
    }

    void toggleMapBit(int blockIndex) {
        int index = getMapIndex(blockIndex);
        free_map[index] = free_map[index] ^ 1u << getShift(blockIndex);
    }


    int nextFreeBlock(int start, int stop) {
        if (start == stop)
            return -1;

        for (int i = start; i < stop; i++) {
            uint32_t bit_map = free_map[getMapIndex(i)] >> getShift(i);
            for (uint32_t j = 0; j < UINT_BITS; j++) {
                bit_map = bit_map >> j;
                if ((bit_map & 1u) % 2 == 0) {
                    return i + j;
                }
            }
        }
        return nextFreeBlock(0, start);
    }

    static int getMapIndex(int blockIndex) {
        return blockIndex / UINT_BITS;
    }

    static uint32_t getShift(int blockIndex) {
        return blockIndex % UINT_BITS;
    }

    void updateFAT(int current, int next) {
        fat_table[current] = next;
    }


    map<string, command_t> commands;
    //const int blockSize;
    int lastBlock = 0;
};


#endif //DISKSIM_FILESYSTEM_H
