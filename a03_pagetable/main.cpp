#include <iostream>
#include "PageTable.h"
#include "level.h"
#include <getopt.h>
#include <vector>
#include <stdexcept>
#include <cstdio>
#include <cstring>

using namespace std;

extern "C" {
#include "byutr.h"
}

int main(int argc, char *argv[]) {
    int samples;
    int *n_value = nullptr;
    char *p_value = nullptr;
    bool t_flag = false;
    int option;

    opterr = 0; // Disable error output for manual error handling
    while ((option = getopt(argc, argv, "n:p:t")) != EOF) {
        switch (option) {
            case 'n':
                try {
                    if ((samples = stoi(optarg, nullptr, 10)) <= 0) {
                        fprintf(stderr,
                                "Option -%c argument must be a number > 0: %s\n",
                                optopt, optarg);
                        return EXIT_FAILURE;
                    }
                }
                catch (const invalid_argument &ia) {
                    cerr << "Option -n argument must be a number." << endl;
                    return EXIT_FAILURE;
                }
                n_value = &samples; // All checks for n value passed
                break;
            case 'p':
                if (strcmp(optarg, "-n") == 0 || strcmp(optarg, "-p") == 0 ||
                    strcmp(optarg, "-t") == 0) {
                    fprintf(stderr,
                            "Option -%c requires a valid argument: %s\n",
                            optopt, optarg);
                    return EXIT_FAILURE;
                }
                p_value = optarg; // All checks for p value passed
                break;
            case 't':
                t_flag = true;
                break;
            case ':':
                fprintf(stderr, "Option -%c requires an argument\n", optopt);
                return EXIT_FAILURE;
            case '?':
                if (isprint(optopt))
                    fprintf(stderr, "Unknown option '-%c'\n", optopt);
                else
                    fprintf(stderr, "Unknown option character '\\x%x'\n",
                            optopt);
                return EXIT_FAILURE;
            default:
                abort();
        }
    }

    if (optind >= argc) {
        cerr << "Expected argument after options" << endl;
        return EXIT_FAILURE;
    } else if (argc - optind < 2) {
        cerr << "Expected at least one page level" << endl;
        return EXIT_FAILURE;
    }

    char *traceFilePath = argv[optind++];
    int remainingArgs = argc - optind;
    //int *levels = new int[remainingArgs];
    int pageBitTotal = 0;
    int levelCount = 0;
    vector<int> levels;

    for (int i = 0; i < remainingArgs; i++) {
        int levelBits;
        try {
            if ((levelBits = stoi(argv[optind], nullptr, 10)) <= 0) {
                fprintf(stderr, "level bit must be a number > 0: %s\n",
                        argv[optind]);
                return EXIT_FAILURE;
            }
        }
        catch (const invalid_argument &ia) {
            if (levelCount == 0) {
                fprintf(stderr, "level bit must be a number: %s\n",
                        argv[optind]);
                return EXIT_FAILURE;
            }


            break;
        }
        levels.push_back(levelBits);
        pageBitTotal += levelBits;
        levelCount++;
        optind++;
    }

    if (pageBitTotal > 30) {
        fprintf(stderr, "Total page bits exceed 30 bits: %d\n", pageBitTotal);
        return EXIT_FAILURE;
    }

    FILE *traceFile;

    // File failed to open
    if ((traceFile = fopen(traceFilePath, "rb")) == nullptr) {
        fprintf(stderr, "Cannot open %s for reading\n", traceFilePath);
        return EXIT_FAILURE;
    }

    //int *levelArray = &levels[levelCount];
    p2AddrTr trace;    // Traced address
    auto pageTable = new PageTable(levelCount, levels);
    int sampleNum = 1;
    while (!feof(traceFile)) {
        if (NextAddress(traceFile, &trace)) {
            uint32_t logicalAddress = trace.addr;
            pageTable->pageInsert(logicalAddress);

            // T-flag was detected
            if (t_flag) {
                uint32_t physicalAddress;
                if (pageTable->logicalToPhysical(logicalAddress,
                                                 physicalAddress))
                    printf("%08x -> %08x\n", logicalAddress, physicalAddress);
            }

            if (n_value && sampleNum == *n_value)
                break;
            sampleNum++;
        }
    }

    fclose(traceFile);

    // P-flag was detected
    if (p_value) {
        FILE *dumpFile;

        // File failed to open
        if ((dumpFile = fopen(p_value, "w")) == nullptr) {
            fprintf(stderr, "Cannot open %s for writing\n", p_value);
            return EXIT_FAILURE;
        }

        // Traverse through page table starting level 0, page 0
        uint32_t page = 0;
        while (page < pageTable->levelInfo[0].entries) {
            pageTable->pageToFrame(dumpFile, page);
            page++;
        }
        fclose(dumpFile);
    }

    // Display summary
    pageTable->displayStats();
    delete pageTable;

    return EXIT_SUCCESS;
}
