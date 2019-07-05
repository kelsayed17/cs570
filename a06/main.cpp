#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <list>
#include <unistd.h>
#include "BlockDevice.h"
#include "SuperBlock.h"
#include "hexdump.h"
#include "FileSystem.h"
#include "DirEntry.h"

#define DEFAULT_BLOCKS 1000
#define COMMENT_LINE 0

using namespace std;

void test_blocks(BlockDevice *device) {
    uint32_t block_size = device->getBlockSize();

    BlockDevice::result result;

    // allocate block buffer
    char *block = new char[block_size];

    const int fill_blocks = 5;
    for (int blk_idx = 0; blk_idx < fill_blocks; blk_idx++) {
        // Fill buffer with data
        char fill_char = 'a' + blk_idx;
        for (int didx = 0; didx < block_size; didx++)
            if (didx == 0)
                block[didx] = '<';
            else if (didx == block_size - 1)
                block[didx] = '>';
            else
                block[didx] = fill_char;

        result = device->writeBlock(blk_idx, block);
        printf("writeBlock %d result %s\n", blk_idx,
               device->resultMessage(result));
    }

    /* Read blocks */
    for (int blk_idx = 0; blk_idx < fill_blocks; blk_idx++) {
        result = device->readBlock(blk_idx, block);
        printf("readBlock %d result %s\n", blk_idx,
               device->resultMessage(result));
        hexDump(block, block_size);
    }

    /* Read block that has not yet been written  */
    int blk = fill_blocks + 10;
    result = device->readBlock(blk, block);

    printf("readBlock %d result %s\n", blk, device->resultMessage(result));

    if (result == BlockDevice::success)
        hexDump(block, block_size);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        cerr << "Requires a minimum of 2 arguments" << endl;
        exit(EXIT_FAILURE);
    } else if (argc > 4) {
        cerr << "Too many arguments" << endl;
        exit(EXIT_FAILURE);
    }

    const char *disk_file = argv[1], *command_file = argv[2];
    int num_blocks = DEFAULT_BLOCKS;

    if (argc == 4) {
        try {
            if ((num_blocks = stoi(argv[3])) < 0) {
                fprintf(stderr, "Option %c must be a positive number: %s\n",
                        'N', argv[3]);
                exit(EXIT_FAILURE);
            }
        }
        catch (const invalid_argument &invalidArgument) {
            fprintf(stderr, "Option %c must be a number: %s\n", 'N', argv[3]);
            exit(EXIT_FAILURE);
        }
        catch (const out_of_range &outOfRange) {
            fprintf(stderr, "Option %c is out of range: %s\n", 'N', argv[3]);
            exit(EXIT_FAILURE);
        }
    }


    struct stat buffer{};

    BlockDevice *sim_disk;
    if (stat(disk_file, &buffer) == FILE_EXISTS) {
        ifstream infile(disk_file);
        if (infile.is_open()) {
            sim_disk = new BlockDevice(disk_file);
        } else {
            cerr << "An error occurred while opening " << command_file << endl;
            exit(EXIT_FAILURE);
        }
    } else {
        sim_disk = new BlockDevice(disk_file, num_blocks);
    }


    auto *fileSystem = new FileSystem(sim_disk);
    if (stat(command_file, &buffer) == FILE_EXISTS) {
        ifstream infile(command_file);
        if (infile.is_open()) {
            string line;
            while (getline(infile, line)) {
                if (!line.empty() && line.rfind('#', 0) != COMMENT_LINE)
                    fileSystem->interpret(line);
            }
            infile.close();
        } else {
            cerr << "An error occurred while opening " << command_file << endl;
            exit(EXIT_FAILURE);
        }
    } else {
        cerr << "Failed to open " << command_file << endl;
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < fileSystem->blockDevice->getBlocks(); i++) {
        fileSystem->blockDevice->dumpBlock(i);
    }
}