//
// Created by Khaled Elsayed on 2019-05-05.
//

#include <sys/stat.h>
#include <fstream>
#include "FileSystem.h"
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include "DirEntry.h"

using namespace std;

result_t FileSystem::format() {
    int block_count = blockDevice->getBlocks();
    int map_size = ceil(block_count / (double) UINT_BITS);
    int map_blocks = ceil(sizeof(uint32_t) * map_size / (double) BLOCK_SIZE);
    int fat_blocks = ceil(sizeof(int) * block_count / (double) BLOCK_SIZE);

    free_map = new uint32_t[map_size]();
    fat_table = new int[block_count];
    uninitialized_fill_n(fat_table, block_count, -1);

    auto *superBlock = new SuperBlock();
    superBlock->free_block = FREEMAP_POSN;
    superBlock->fat_block = FAT_POSN;
    superBlock->root_dir_block = SUPERBLOCK_POSN + 1;
    superBlock->root_dir_bytes = BLOCK_SIZE;

    char buffer[BLOCK_SIZE];
    memcpy(buffer, &superBlock, sizeof superBlock);
    blockDevice->writeBlock(SUPERBLOCK_POSN, buffer);

    toggleMapBit(SUPERBLOCK_POSN);

    free_map_size = map_size;

    initialize(FREEMAP_POSN, FREEMAP_POSN + map_blocks, block_count);
    initialize(FAT_POSN, FAT_POSN + fat_blocks, block_count);

    return SUCCESS;
}

result_t FileSystem::place(const string &src_file, const string &dest_path) {
    int block_count = blockDevice->getBlocks();
    const int block_size = blockDevice->getBlockSize();

    int currentBlock = nextFreeBlock(getMapIndex(lastBlock), block_count);

    if (currentBlock == -1)
        return FAILURE;

    struct stat statBuffer{};
    if (stat(src_file.c_str(), &statBuffer) == FILE_EXISTS) {
        ifstream infile(src_file);

        if (infile.is_open()) {
            if (dest_path.size() > MAXFNAMELEN)
                return FAILURE;

            // Set file metadata
            DirEntry dirEntry;
            dirEntry.directory = false;
            dirEntry.start_blk = currentBlock;
            strncpy(dirEntry.fname, dest_path.c_str(), dest_path.size());
            infile.seekg(0, infile.end);
            dirEntry.size = infile.tellg();
            infile.seekg(0, infile.beg);

            int header_size = sizeof dirEntry;
            int total_size = header_size + dirEntry.size;
            int blocks_needed = (total_size / block_size) + 1;

            char buffer[block_size];
            for (int i = 0; i < blocks_needed; i++) {

                if (i == 0) {
                    memcpy(buffer, &dirEntry, header_size);

                    int remaining = block_size - header_size;

                    infile.read(&buffer[header_size], remaining);

                    blockDevice->writeBlock(currentBlock, buffer);

                } else {
                    infile.read(buffer, block_size);
                    blockDevice->writeBlock(currentBlock, buffer);
                }

                toggleMapBit(currentBlock);
                int next = nextFreeBlock(currentBlock, block_count);

                if (next == -1)
                    return FAILURE;

                updateFAT(currentBlock, next);
                currentBlock = next;
            }
            lastBlock = currentBlock;

            infile.close();
        } else {
            cerr << "An error occurred while opening " << src_file << endl;
            exit(EXIT_FAILURE);
        }
    }

    return SUCCESS;
}

result_t FileSystem::list() {
    return SUCCESS;
}

result_t FileSystem::listblocks() {
    return SUCCESS;
}

result_t FileSystem::mkdir(const string &path) {
    return SUCCESS;
}

result_t FileSystem::remove() {
    return SUCCESS;
}

result_t FileSystem::dump() {
    return SUCCESS;
}
