#include <getopt.h>
#include <stdexcept>
#include "Producer.h"
#include "Consumer.h"

#define THREAD_COUNT 4
#define SUCCESS 0
#define LUCY_DELAY_DEFAULT 0
#define ETHEL_DELAY_DEFAULT 0
#define FROG_DELAY_DEFAULT 0
#define ESCARGOT_DELAY_DEFAULT 0

char USAGE[] = " [-L N (default %d)] [-E N (default %d)] [-f N (default %d)] [-e N (default %d)]\n"
               "\t-h : report usage (this message)\n"
               "\t-L N : N is the number of milliseconds to delay Lucy consumer\n"
               "\t-E N : N is the number of milliseconds to delay Ethel consumer\n"
               "\t-f N : N is the number of milliseconds to delay frog producer\n"
               "\t-e N : N is the number of milliseconds to delay escargot producer\n";

int convertArg() {
    int number;
    try {
        if ((number = stoi(optarg)) < 0) {
            fprintf(stderr, "Option -%c must be a positive number: %s\n", optopt, optarg);
            exit(EXIT_FAILURE);
        }
    }
    catch (const invalid_argument &invalidArgument) {
        fprintf(stderr, "Option -%c must be a number: %s\n", optopt, optarg);
        exit(EXIT_FAILURE);
    }
    catch (const out_of_range &outOfRange) {
        fprintf(stderr, "Option -%c is out of range: %s\n", optopt, optarg);
        exit(EXIT_FAILURE);
    }

    // All checks passed
    return number;
}

int main(int argc, char *argv[]) {
    int E_value = 0, L_value = 0, f_value = 0, e_value = 0;
    int option;
    opterr = 0;
    while ((option = getopt(argc, argv, "E:L:f:e:")) != EOF) {
        switch (option) {
            case 'E':
                E_value = convertArg();
                break;
            case 'L':
                L_value = convertArg();
                break;
            case 'f':
                f_value = convertArg();
                break;
            case 'e':
                e_value = convertArg();
                break;
            default:
                fprintf(stderr, "Usage: %s : ", argv[0]);
                fprintf(stderr, USAGE, LUCY_DELAY_DEFAULT, ETHEL_DELAY_DEFAULT, FROG_DELAY_DEFAULT,
                        ESCARGOT_DELAY_DEFAULT);
                exit(EXIT_FAILURE);
        }
    }

    // Creating shared buffer
    auto *buffer = new Buffer();

    // Creating producer and consumer threads
    Consumer lucy(buffer, Lucy, "Lucy", L_value);
    Consumer ethel(buffer, Ethel, "Ethel", E_value);
    Producer frog(buffer, Frog, "crunchy frog bite", f_value);
    Producer escargot(buffer, Escargot, "escargot sucker", e_value);

    Thread *threads[THREAD_COUNT] = {&frog, &escargot, &ethel, &lucy};

    for (auto thread : threads) {
        if (thread->start() != SUCCESS) {
            fprintf(stderr, "Unable to create %s thread\n", thread->getName());
            exit(EXIT_FAILURE);
        }
    }

    // Main thread waits here until last item is consumed
    pthread_barrier_wait(&buffer->barrier);

    for (auto thread : threads) {
        // Wait for each thread to exit
        if (thread->join() != SUCCESS) {
            fprintf(stderr, "Thread join error: %s\n", thread->getName());
            exit(EXIT_FAILURE);
        }
    }

    printf("\n");
    printf("PRODUCTION REPORT\n");
    printf("----------------------------------------\n");
    printf("%s producer generated %d candies\n", frog.getName(), frog.getProducedCount());
    printf("%s producer generated %d candies\n", escargot.getName(), escargot.getProducedCount());
    printf("%s consumed %d %ss + %d %ss = %d\n", lucy.getName(), lucy.getConsumedFrogCount(), frog.getName(),
           lucy.getConsumedEscargotCount(), escargot.getName(),
           lucy.getConsumedFrogCount() + lucy.getConsumedEscargotCount());
    printf("%s consumed %d %ss + %d %ss = %d\n", ethel.getName(), ethel.getConsumedFrogCount(), frog.getName(),
           ethel.getConsumedEscargotCount(), escargot.getName(),
           ethel.getConsumedFrogCount() + ethel.getConsumedEscargotCount());

    delete buffer;
    return EXIT_SUCCESS;
}