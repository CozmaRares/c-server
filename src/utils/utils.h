#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdbool.h>

#include "ds/dict.h"
#include "ds/queue.h"

#define MALLOC(type, name, size)                                                              \
    name = (type*)malloc((size) * sizeof(type));                                              \
    if (name == NULL) {                                                                       \
        err_n_die("Could not allocate enough memory for " #type ", at line: %d\n", __LINE__); \
    }

#define CALLOC(type, name, size)                                                              \
    name = (type*)calloc((size), sizeof(type));                                               \
    if (name == NULL) {                                                                       \
        err_n_die("Could not allocate enough memory for " #type ", at line: %d\n", __LINE__); \
    }

void err_n_die(const char* const fmt, ...);
int to_double(const char* const str, double* const dest, bool strict);
char* new_string(const char* const str);

#endif  // __UTILS_H__
