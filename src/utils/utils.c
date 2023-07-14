#include "utils.h"

#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void err_n_die(const char* const fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
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

char* new_string(const char* const str) {
    size_t len = strlen(str);

    char* new_str;
    MALLOC(char, new_str, len + 1);
    memcpy(new_str, str, len);
    new_str[len] = '\0';

    return new_str;
}

typedef struct stat stat_t;

char* read_file(const char* const path) {
    stat_t status;

    if (stat(path, &status) == -1 || !S_ISREG(status.st_mode))
        return NULL;

    int file_fd = open(path, O_RDONLY);

    if (file_fd == -1)
        return NULL;

    char* contents;
    MALLOC(char, contents, status.st_size + 1);
    read(file_fd, contents, status.st_size);
    contents[status.st_size] = '\0';

    return contents;
}
