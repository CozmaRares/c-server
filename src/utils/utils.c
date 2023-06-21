#include "utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void err_n_die(const char* const fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    exit(1);
}

int to_double(const char* const str, double* const dest, bool strict) {
    if (str[0] == '\0')
        return 1;

    char* end = NULL;
    double d  = strtod(str, &end);

    if (end == str || (strict && end[0] != '\0'))
        return 1;

    *dest = d;
    return 0;
}
