#ifndef UTILS_H_
#define UTILS_H_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUCKET_SIZE 64

/* useful macro for handling error codes */
#define DIE(assertion, call_description)                                       \
    do {                                                                       \
        if (assertion) {                                                       \
            fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                 \
            perror(call_description);                                          \
            exit(errno);                                                       \
        }                                                                      \
    } while (0)

/* Functions used for comparing different data types */
int
compare_function_ints(void *a, void *b);

int
compare_function_strings(void *a, void *b);

/* Hashing functions */
unsigned int
hash_function_int(void *a);

unsigned int
hash_function_string(void *a);

#endif /* UTILS_H_ */
