#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "BlockDevice.h"
#include "SuperBlock.h"
#include "hexdump.h"


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

    const char *disk;

    if (argc >= 2)
        disk = argv[1];
    else
        disk = "foobar.dsk"; // default disk

    uint32_t numblocks = 100;

    BlockDevice *device;

    // See if the the disk exists
    struct stat statbuf;
    if (stat(disk, &statbuf) == -1) {
        // couldn't stat, try to create  new disk
        device = new BlockDevice(disk, numblocks);
    } else {
        // open existing
        device = new BlockDevice(disk);
    }

    test_blocks(device);

    return 0;
}
