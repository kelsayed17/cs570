#ifndef DIRENTRY_H
#define DIRENTRY_H

#define MAXFNAMELEN 256
typedef struct {
    uint32_t start_blk;    // first block of file
    uint32_t size;    // size of file in bytes
    bool directory;    // true:directory, false:file
    char fname[MAXFNAMELEN];
} DirEntry;

#endif
