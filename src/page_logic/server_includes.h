#ifndef __SERVER_INCLUDES_H__
#define __SERVER_INCLUDES_H__

#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../utils/utils.h"
#include "includes.h"

int create_pipe() {
    return mkfifo(PIPE, 0666);
}

int open_pipe() {
    return open(PIPE, O_RDONLY);
}

bool load_entry(const int pipefd, dict_t* const dict) {
    size_t size;

    read(pipefd, &size, sizeof(size_t));

    if (size == 0)
        return false;

    char* key;
    MALLOC(char, key, size + 1);
    read(pipefd, key, size);
    key[size] = '\0';

    read(pipefd, &size, sizeof(size_t));
    char* value;
    MALLOC(char, value, size + 1);
    read(pipefd, value, size);
    value[size] = '\0';

    dict_set(dict, key, value);

    free(key);
    free(value);

    return true;
}

void close_pipe(const int pipefd) {
    close(pipefd);
    unlink(PIPE);
}

#endif  // __SERVER_INCLUDES_H__
