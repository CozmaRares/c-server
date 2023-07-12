#include "http.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../utils/ds/dict.h"
#include "../utils/ds/queue.h"
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

char* get_response_message(const http_status_code_t status) {
    switch (status) {
        case OK:
            return "OK";
        case CREATED:
            return "CREATED";
        case NO_CONTENT:
            return "NO_CONTENT";
        case PARTIAL_CONTENT:
            return "PARTIAL_CONTENT";
        case MOVED_PERMANENTLY:
            return "MOVED_PERMANENTLY";
        case FOUND:
            return "FOUND";
        case TEMPORARY_REDIRECT:
            return "TEMPORARY_REDIRECT";
        case PERMANENT_REDIRECT:
            return "PERMANENT_REDIRECT";
        case BAD_REQUEST:
            return "BAD_REQUEST";
        case NOT_FOUND:
            return "NOT_FOUND";
        case URI_TOO_LONG:
            return "URI_TOO_LONG";
        case REQUEST_HEADER_FIELDS_TOO_LARGE:
            return "REQUEST_HEADER_FIELDS_TOO_LARGE";
        case INTERNAL_SERVER_ERROR:
            return "INTERNAL_SERVER_ERROR";
        case NOT_IMPLEMENTED:
            return "NOT_IMPLEMENTED";
        case HTTP_VERSION_NOT_SUPPORTED:
            return "HTTP_VERSION_NOT_SUPPORTED";
    }

    return NULL;
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

    destroy_queue(&fields);
}

char* create_http_request(char* const request, http_request_t* const dest) {
    for (int i = 1; request[i]; i++)
        if (request[i - 1] == '\n' && request[i] == '\n')
            request[i] = '|';

    char* request_line = strtok(request, "\n");
    char* headers      = strtok(NULL, "|");
    char* body         = strtok(NULL, "|");

    http_method_t method;
    if (get_method(strtok(request_line, " "), &method))
        return "Unknown http method";

    char* uri = strtok(NULL, " ");

    double version;
    strtok(NULL, "/");
    if (to_double(strtok(NULL, ""), &version, false))
        return "Invalid version number";

    dest->method  = method;
    dest->version = version;
    dest->uri     = new_string(uri);
    dest->body    = body ? new_string(body) : NULL;
    dest->headers = create_default_dict();
    parse_headers(headers, dest->headers);

    return NULL;
}

void destroy_http_request(http_request_t* const req) {
    free(req->uri);
    destroy_dict(&req->headers);
    free(req->body);
}

int pipefd[2];
#define READ_PIPE  0
#define WRITE_PIPE 1

void write_dict_entry(const dict_entry_t* const entry) {
    int key_len    = strlen(entry->key);
    int value_len  = strlen(entry->value);
    int total_size = key_len + value_len + 3;

    write(pipefd[WRITE_PIPE], &total_size, sizeof(int));
    write(pipefd[WRITE_PIPE], entry->key, key_len);
    write(pipefd[WRITE_PIPE], ": ", 2);
    write(pipefd[WRITE_PIPE], entry->value, value_len);
    write(pipefd[WRITE_PIPE], "\n", 1);
}

http_response_t create_http_response() {
    http_response_t res;
    res.version = 1.1;
    res.body    = NULL;
    res.headers = create_default_dict();
    return res;
}

void destroy_http_response(http_response_t* const response) {
    destroy_dict(&response->headers);
    free(response->body);
}

char* http_request_to_string(const http_response_t* const response) {
    long long size = 0;
    char* res;
    MALLOC(char, res, 256 * 1024);

    // put version
    size += sprintf(res, "HTTP/%.2g ", response->version);

    char* response_message = get_response_message(response->status);

    if (response_message == NULL) {
        fprintf(stderr, "\n === ERROR === \nUnknown status code: %d\n\n", response->status);
        sprintf(res + size, "%d %s\n\n", INTERNAL_SERVER_ERROR, get_response_message(INTERNAL_SERVER_ERROR));
        return res;
    }

    // put status
    size += sprintf(res + size, "%d %s\n", response->status, response_message);

    // put headers
    pipe(pipefd);

    dict_for_each(response->headers, write_dict_entry);
    close(pipefd[WRITE_PIPE]);

    int bytes;

    while (read(pipefd[READ_PIPE], &bytes, sizeof(int)) > 0)
        size += read(pipefd[READ_PIPE], res + size, bytes);

    size += sprintf(res + size, "\n");

    // put body
    if (response->body)
        sprintf(res + size, "%s\n\n", response->body);

    return res;
}
