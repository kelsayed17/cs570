#include <sys/stat.h>
#include <math.h>
#include <stdexcept>

#include "BlockDevice.h"

using namespace std;

BlockDevice::BlockDevice(const char *filename, uint32_t blocks,
                         uint32_t block_size) {

    // If block_size is specified, ensure that it is divisible by 32 bits
    // Real devices are usually a power of 2
    if (block_size && (block_size % sizeof(int32_t) != 0))
        throw runtime_error("Device block size must be a multiple of 4 bytes");

    size_t count = 0;  // header fields read/written

    // We really only need a a few bytes (sizeof(device_meta) for the device metadata
    // However, it will make our life much simpler if we extend that too the standard
    // block size as everything will be aligned.
    // Find the number of blocks we need to hold the device meta information
    uint32_t complete_blocks = sizeof(device_meta) / block_size;
    uint32_t partial_blocks = sizeof(device_meta) % block_size > 0 ? 1 : 0;
    this->offset = (complete_blocks + partial_blocks) * block_size;

    size_t expected_hdr_count = 2;  // # fields in hdr

    if (blocks == 0) {
        // User did not specify device characteristics, assume existing
        // block device and check if simulation file exists.
        struct stat status;
        bool exists = stat(filename, &status) != -1;
        if (!exists)
            throw runtime_error("File does not exist");

        // open and read metadata
        this->file_h = fopen(filename, "r+");
        if (this->file_h == NULL)
            throw runtime_error("Unable to open file");

        // Read in metadata and verify that we read the expected amount.
        count += fread(&this->block_size, sizeof(uint32_t), 1, this->file_h);
        count += fread(&this->blocks, sizeof(uint32_t), 1, this->file_h);
        if (count != expected_hdr_count)
            throw runtime_error("Unable to read device metadata");

    } else {
        // New device
        this->file_h = fopen(filename, "w+b");
        if (this->file_h == NULL)
            throw runtime_error("Unable to create new device");

        this->blocks = blocks;
        this->block_size = block_size;
        // Write header
        count += fwrite(&this->block_size, sizeof(uint32_t), 1, this->file_h);
        count += fwrite(&this->blocks, sizeof(uint32_t), 1, this->file_h);
        if (count != expected_hdr_count)
            throw runtime_error("Unable to initialize device metadata");
    }
}

BlockDevice::~BlockDevice() {
    if (this->file_h != NULL) {
        // We check the result of the synchronize, but we have
        // already started the destructor and there is no turning back.
        // We don't check it as there is nothing that we can do about it.
        this->synchronize();
        // release resources
        fclose(this->file_h);
        this->file_h = NULL;
    }
}


// ------------ private functions ---------------------------
/* uint32_t lastBlock()
   Returns index of last block implemented by file system
   The number of blocks supported by the device may be higher
*/
uint32_t BlockDevice::lastBlock() {
    /* seek to end and divide position by block size
     * after accounting for metadata at the beginning
     * of the file
     */
    int position = fseek(this->file_h, 0, SEEK_END);
    int32_t n = static_cast<int32_t>(
            (position - this->offset) / this->block_size);
    return n;
}

/* result seekBlock(unit32_t block, int offset=0)
 * Seek to offset bytes within the specified block.
 * Returns
 *   success - Ready to read or write specified block
 *   nosuchblock - Block number not in device's pool of blocks
 *   badblock - Cannot access block (device error)
 */
BlockDevice::result BlockDevice::seekBlock(uint32_t block, int offset = 0) {
    // check for valid block number
    BlockDevice::result retval =
            (block < this->blocks) ? BlockDevice::success
                                   : BlockDevice::nosuchblock;

    int retseek;  // seek result
    if (retval == BlockDevice::success) {
        /* valid block number */

        // compute start byte and seek to it
        long block_start = this->offset + block * this->block_size;
        retseek = fseek(this->file_h, block_start + offset, SEEK_SET);
        retval = (retseek == 0) ? BlockDevice::success : badblock;
    }

    return retval;
}

// ------------ public functions ---------------------------

/* Complete any pending writes */
BlockDevice::result BlockDevice::synchronize() {

    BlockDevice::result retval;

    // flush write buffer (0 == success)
    retval = (fflush(this->file_h) == 0) ?
             BlockDevice::success : BlockDevice::nosynch;
    return retval;
}

BlockDevice::result BlockDevice::readBlock(uint32_t block,
                                           void *buffer) {
    /* seek to block and read it */

    BlockDevice::result retval = this->seekBlock(block);

    if (retval == BlockDevice::success) {
        /* successful seek, try to read */
        size_t read = fread(buffer, this->block_size, 1, this->file_h);
        retval = (read == 1) ?
                 BlockDevice::success : BlockDevice::badreadwrite;

        if (retval == BlockDevice::badreadwrite) {
            // We might have failed because the file is shorter
            // than the virtual device.  Seek to the last byte
            // of the block which extends the file, then try again.
            BlockDevice::result seekend = this->seekBlock(block,
                                                          this->block_size - 1);


            if (seekend == BlockDevice::success) {
                // Write a character to the last byte of block to extend the file
                char nullchar = 0;
                retval = (fwrite(&nullchar, sizeof(char), 1, this->file_h) == 1)
                         ?
                         BlockDevice::success : BlockDevice::badreadwrite;
                // seek to beginning of block
                if (retval == BlockDevice::success)
                    retval = this->seekBlock(block);
            }

            if (retval == BlockDevice::success) {
                // read one block of block size bytes and verify write
                size_t read = fread(buffer, this->block_size, 1, this->file_h);
                retval = (read == 1) ?
                         BlockDevice::success : BlockDevice::badreadwrite;
            }
        }
    }

    return retval;

}

BlockDevice::result BlockDevice::writeBlock(uint32_t block,
                                            const void *buffer) {

    /* seek to block and write it */
    BlockDevice::result retval = BlockDevice::seekBlock(block);
    if (retval == BlockDevice::success) {
        // write one block of block size bytes and verify write
        size_t written = fwrite(buffer, this->block_size, 1, this->file_h);
        retval = (written == 1) ?
                 BlockDevice::success : BlockDevice::badreadwrite;
    }

    return retval;
}


/*
 * Given a BlockDevice::result, return a human interpretalble message
 */
const char *BlockDevice::resultMessage(BlockDevice::result code) {
    return this->messages[code];
}


// -------------------- data -------------------------
/*
 * Human readable result strings
 * Must correspond to BlockDevice::result
 */
const char *BlockDevice::messages[] = {
        "operation succeeded",
        "block does not exist",
        "bad block",  // can't seek to it
        "unable to read/write block",
        "unable to synchornize",
};

