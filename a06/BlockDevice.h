#ifndef BLOCKDEVICE_H
#define BLOCKDEVICE_H

#include <stdint.h>
#include <stdio.h>

#include "hexdump.h"

/*
 * meta information for simulated device.
 * We store this at the beginning of the file
 * to simulate device characteristics.
 */

typedef struct {
    uint32_t block_size;
    uint32_t blocks;
} device_meta;

/* BlockDevice
 * Simulates a block device (e.g. solid-state/magnetic drive)
 * on a file.  The file will not necessarily be the same
 * size as the device as we only allocate physical space up to 
 * and including the last used block.
 */
class BlockDevice {

public:
    /*
    * Success and error codes.
    * If these are changed, be sure to change human/readable
    * message string array in class definition.
    */
    typedef enum {
        success,    // operation completed successfully
        nosuchblock,  // bad block index
        badblock,     // cannot access block
        badreadwrite,  // unable to complete read or write
        nosynch,    // unable to synchronize buffers with blocks
    } result;
    // messages corresponding to results
    // values declared in class definition
    static const char *messages[];


private:
    uint32_t block_size; // space per block in bytes
    uint32_t blocks; // number of blocks in device
    uint32_t file_blocks;  // # file blocks simulating device (may be < blocks)

    const char *filename;  // file simulating device
    FILE *file_h;  // handle to file

    // data starts offset bytes into file after the meta
    // information that describes the simulated device.
    uint32_t offset;


    /* uint32_t lastBlock()
       Returns index of last block implemented by file system
       The number of blocks supported by the device may be higher
     */
    uint32_t lastBlock();


    /* result seekBlock(unit32_t block, uint32_t offset)
     * Seek offset bytes to the specified block.
     * Usually called with offset 0 (default).
     * The offset is mainly used to force the simulated device
     * to allocate to the end of the block.  If the block is not yet
     * simulated, seeking to the start of the block and reading will cause
     * an error.  By seeking to the end of the block first, we can force
     * the file length to increase.  (True on UNIX systems.)
     * Returns
     *   success - Ready to read or write specified block
     *   nosuchblock - Block number not in device's pool of blocks
     *   badblock - Cannot access block (device error)
     */
    result seekBlock(uint32_t block, int offset);

public:


    /*
      * BlockDevice(const char *filename, uint32_t blocks, uint32_t
      * block_size)
      * Create or open a simulated block I/O device backed by the
      * specified filename.  If blocks > 0, the device is created,
      * otherwise we try to open the device.  (We don't use two different
      * constructors to ensure C compatibility.)
      */
    BlockDevice(const char *filename, uint32_t blocks = 0,
                uint32_t block_size = 4096);

    /* ~BlockDevice() - close the block device, writing any pending blocks */
    ~BlockDevice();

    /* result readBlock(unit32_t block, void* buffer)
     * Read the specified block.  Populates the specified block_size buffer with
     * data if successful.
     * Return result indicating success or failure reason.
     */
    result readBlock(uint32_t block, void *buffer);

    /* result writeblock(uint32_t block, const void * data)
     * Write data[block_size] to the specified block number.
     * Return:  result indicates success or problem type
     */
    result writeBlock(uint32_t block, const void *buffer);

    /*
     * void dumpBlock(uint32_t block)
     * Provide hexadecimal dump of block
     */
    inline void dumpBlock(uint32_t block) {
        // Read block into buffer
        uint8_t buffer[this->block_size];
        result retval = this->readBlock(block, buffer);
        if (retval == success) {
            hexDump(buffer, this->block_size);
        } else
            printf("Unable to dump block %d: %s\n",
                   block, this->resultMessage(retval));
    }

    /* result synchronize()
     * Flush any pending writes to disk.
     * result indicates success or failure reason
     */
    result synchronize();

    /*
     * Given a BlockDevice::result, return a human interpretalble message
     */
    const char *resultMessage(result r);

    /* getBlockSize() - Return size of blocks in bytes */
    inline uint32_t getBlockSize() {
        return this->block_size;
    }

    /* getBlocks() - Return number of blocks on device */
    inline uint32_t getBlocks() {
        return this->blocks;
    }


};

#endif
