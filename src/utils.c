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
