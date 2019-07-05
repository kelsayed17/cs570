#ifndef HEXDUMP_H
#define HEXDUMP_H

/*
 * hexDump
 * Write a size byte block of data to stdout as a hexadecimal block.  Example:
 * hexDump(buf, 1024)
 */
void hexDump(const void* data, size_t size);

#endif
