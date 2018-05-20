#ifndef FREEPOI_TEST_H
#define FREEPOI_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALL_CLEAR    (!failed ? EXIT_SUCCESS : EXIT_FAILURE)

static int failed;

#define print_test_message(a)       \
    do {                            \
        fprintf(stderr, a "... ");  \
        fflush(stderr);             \
    } while(0)

#define test___(c)                      \
    do {                                \
        if (!(c)) {                     \
            fprintf(stderr, "FAIL\n");  \
            failed++;                   \
        } else {                        \
            fprintf(stderr, "PASS\n");  \
        }                               \
        fflush(stderr);                 \
    } while(0)

#define test_verbose(cond, ...)                             \
    do {                                                    \
        if (!(cond)) {                                      \
            fprintf(stderr, "FAIL\n\t" __VA_ARGS__);    \
            failed++;                                       \
        } else {                                            \
            fprintf(stderr, "PASS\n");                  \
        }                                                   \
        fflush(stderr);                                     \
    } while(0)

#define is_equal(a, b) test___((a) == (b))
#define memcmp_test(a, b, s) test___(!memcmp((a), (b), (s)))

#endif
