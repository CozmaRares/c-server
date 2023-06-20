#include "http.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

#define STR(literal) #literal

// lmao blaze it
#define METH(name) \
    { .method = name, .method_name = STR(name) }

int get_method(char* method, http_method_t* const dest) {
    struct {
        http_method_t method;
        char* method_name;
    } http_methods[] = {
        METH(GET),
        METH(PUT),
        METH(HEAD),
        METH(POST),
        METH(PATCH),
        METH(TRACE),
        METH(DELETE),
        METH(CONNECT),
        METH(OPTIONS)
    };
    int size = sizeof(http_methods) / sizeof(http_methods[0]);

    for (int i = 0; i < size; i++)
        if (strcmp(method, http_methods[i].method_name) == 0) {
            *dest = http_methods[i].method;
            return 0;
        }

    return 1;
}

int create_http_request(char* request, http_request_t* const dest) {
    for (int i = 1; request[i]; i++)
        if (request[i - 1] == '\n' && request[i] == '\n')
            request[i] = '|';

    char* request_line = strtok(request, "\n");
    char* headers      = strtok(NULL, "|");
    char* body         = strtok(NULL, "|");

    http_method_t method;
    if (get_method(strtok(request_line, " "), &method)) {
        fprintf(stderr, "Unknown http method\n");
        return 1;
    }

    char* uri = strtok(NULL, " ");

    double version;
    strtok(NULL, "/");
    if (to_double(strtok(NULL, ""), &version, false)) {
        fprintf(stderr, "Invalid version number\n");
        return 1;
    }

    dest->method  = method;
    dest->uri     = uri;
    dest->version = version;

    return 0;
}
