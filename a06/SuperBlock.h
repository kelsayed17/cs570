
#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

#include <stdint.h>

/* block containing superblock
 * normally we would have a backup data structures
 * but will not so here
 */
#define SUPERBLOCK_POSN 0
#define FREEMAP_POSN 100
#define FAT_POSN 200

#define TINYFS "TinyFS"

typedef struct {
    char magic[sizeof(TINYFS)];  /* magic format identifier */
    uint32_t free_block;  /* first block of freemap block */
    uint32_t fat_block;   /* first block of file allocation table */
    /* Root directory: first block and directory size */
    uint32_t root_dir_block;
    uint32_t root_dir_bytes;
} SuperBlock;


#endif
