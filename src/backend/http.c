#include "http.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"

// lmao blaze it
#define METH(name) \
    { .method = name, .method_name = #name }

int get_method(const char* const method, http_method_t* const dest) {
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

void parse_headers(char* const headers, dict_t* const dict) {
    queue_t* fields = create_queue();

    char* line = strtok(headers, "\r");

    while (line) {
        if (line[0] == '\n')
            line++;

        if (line[0] == '\0')
            break;

        enqueue(fields, line);
        line = strtok(NULL, "\r");
    }

    while (!queue_is_empty(fields)) {
        dequeue(fields, &line);

        char* key   = strtok(line, ":");
        char* value = strtok(NULL, "");

        // discard first space
        if (value[0] == ' ')
            value++;

        dict_set(dict, key, value);
        free(line);
    }

    free_queue(&fields);
}

char* create_http_request(char* const request, http_request_t* const dest) {
    for (int i = 1; request[i]; i++)
        if (request[i - 1] == '\n' && request[i] == '\n')
            request[i] = '|';

    char* request_line = strtok(request, "\n");
    char* headers      = strtok(NULL, "|");
    // char* body         = strtok(NULL, "|");

    http_method_t method;
    if (get_method(strtok(request_line, " "), &method))
        return "Unknown http method";

    char* uri = strtok(NULL, " ");

    double version;
    strtok(NULL, "/");
    if (to_double(strtok(NULL, ""), &version, false))
        return "Invalid version number";

    dest->method  = method;
    dest->uri     = new_string(uri);
    dest->version = version;

    dest->headers = create_default_dict();
    parse_headers(headers, dest->headers);

    return NULL;
}

void free_http_request(http_request_t* const req) {
    free(req->uri);
    free_dict(&req->headers);
}
