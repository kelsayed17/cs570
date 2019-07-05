#include <stdio.h>

/* hexDump
 * modified from Chris Brown's gist
 * https://gist.github.com/ccbrown/9722406
 */

void hexDump(const void *data, size_t size) {

    /* Number of bytes per line, groupings and character representation */
    const int charsperline = 16;
    const int seperateat = 8;
    char ascii[charsperline + 1];
    ascii[charsperline] = '\0';  /* Initialize null at end */
    /* data buffer cast as unsigned character array */
    unsigned char *buffer = (unsigned char *) data;

    /* Determine how many hex nibbles to use for offset
     * and create format string for offset into buffer
     */
    int nibbles = 0;
    const int bits_per_nibble = 4;
    int tmpsize = size;
    while (tmpsize > 0) {
        nibbles += 1;
        tmpsize = tmpsize >> bits_per_nibble;
    }
    char fmt_string[10];
    sprintf(fmt_string, "%%0%dX ", nibbles);

    /* iterate through the buffer */
    size_t i, j;
    for (i = 0; i < size; ++i) {

        if (i % charsperline == 0) {
            /* At start of line, show offset into data */
            char buf[100];
            sprintf(buf, fmt_string, i);

            printf(fmt_string, i);
        }

        /* dump hex value */
        printf("%02X ", buffer[i]);

        /* Populate ASCII buffer.
         * use '.' if not a printable character
         * buffer will be dumped at line end
         */
        if (buffer[i] >= ' ' && buffer[i] <= '~') {
            ascii[i % charsperline] = buffer[i];
        } else {
            ascii[i % charsperline] = '.';
        }

        if ((i + 1) % seperateat == 0 || i + 1 == size) {
            /* Add space when seperateat nibbles have gone by */
            printf(" ");
            if ((i + 1) % charsperline == 0) {
                /* At end of line, write ascii */
                printf("|  %s \n", ascii);
            } else if (i + 1 == size) {
                /* Last line is shorter */
                ascii[(i + 1) % charsperline] = '\0';  /* terminate ASCII */
                /* space past remaining hex dump */
                if ((i + 1) % charsperline <= seperateat) {
                    printf(" ");
                }
                for (j = (i + 1) % charsperline; j < charsperline; ++j) {
                    printf("   ");
                }
                /* write ASCII */
                printf("|  %s \n", ascii);
            }
        }
    }
}
