#ifndef __PAGE_INCLUDES_H__
#define __PAGE_INCLUDES_H__

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "includes.h"

int open_pipe() {
    return open(PIPE, O_WRONLY);
}

void send_entry(const int pipefd, const char* const key, const char* const value) {
    size_t l = strlen(key);
    write(pipefd, &l, sizeof(size_t));
    write(pipefd, key, l);

    l = strlen(value);
    write(pipefd, &l, sizeof(size_t));
    write(pipefd, value, l);
}

void close_pipe(const int pipefd) {
    size_t a = 0;

    write(pipefd, &a, sizeof(size_t));
    close(pipefd);
}

#endif  // __PAGE_INCLUDES_H__
