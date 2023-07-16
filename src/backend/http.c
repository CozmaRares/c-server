#include "http.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../utils/ds/dict.h"
#include "../utils/ds/queue.h"
#include "../utils/utils.h"

int get_method(const char* const method, http_method_t* const dest);
char* get_response_message(const http_status_code_t status);
void parse_headers(char* const headers, dict_t* const dict);
void parse_query(char* const query, dict_t* const dict);
void write_dict_entry(const dict_entry_t* const entry);

http_status_code_t create_http_request(char* const request, http_request_t* const dest) {
    for (int i = 1; request[i]; i++)
        if (request[i - 1] == '\n' && request[i] == '\n')
            request[i] = '|';

    char* request_line = strtok(request, "\n");
    char* headers      = strtok(NULL, "|");
    char* body         = strtok(NULL, "|");

    http_method_t method;
    if (get_method(strtok(request_line, " "), &method))
        return NOT_IMPLEMENTED;

    char* uri = strtok(NULL, " ");

    double version;
    strtok(NULL, "/");
    if (to_double(strtok(NULL, ""), &version, false) || version - 1.1 > 0.001)
        return HTTP_VERSION_NOT_SUPPORTED;

    dest->method  = method;
    dest->version = version;
    dest->body    = body ? new_string(body) : NULL;
    dest->headers = create_default_dict();
    dest->query   = create_default_dict();
    parse_headers(headers, dest->headers);

    dest->url   = new_string(strtok(uri, "?"));
    char* query = strtok(NULL, "");
    parse_query(query, dest->query);

    return OK;
}

void destroy_http_request(http_request_t* const req) {
    free(req->url);
    destroy_dict(&req->headers);
    destroy_dict(&req->query);
    free(req->body);
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

int pipefd[2];
#define READ_PIPE  0
#define WRITE_PIPE 1

char* http_response_to_string(const http_response_t* const response) {
    long long size = 0;
    char* res;
    MALLOC(char, res, 256 * 1024);

    // put version
    size += sprintf(res, "HTTP/%g ", response->version);

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

    size_t bytes;

    while (read(pipefd[READ_PIPE], &bytes, sizeof(size_t)) > 0)
        size += read(pipefd[READ_PIPE], res + size, bytes);

    size += sprintf(res + size, "\n");

    // put body
    if (response->body)
        sprintf(res + size, "%s", response->body);

    return res;
}

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
            return "Created";
        case NO_CONTENT:
            return "No Content";
        case PARTIAL_CONTENT:
            return "Partial Content";
        case MOVED_PERMANENTLY:
            return "Moved Permanently";
        case FOUND:
            return "Found";
        case TEMPORARY_REDIRECT:
            return "Temporary Redirect";
        case PERMANENT_REDIRECT:
            return "Permanent Redirect";
        case BAD_REQUEST:
            return "Bad Request";
        case NOT_FOUND:
            return "Not Found";
        case INTERNAL_SERVER_ERROR:
            return "Internal Server Error";
        case NOT_IMPLEMENTED:
            return "Not Implemented";
        case HTTP_VERSION_NOT_SUPPORTED:
            return "HTTP Version Not Supported";
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

        DICT_SET_STRING(dict, key, value);
        free(line);
    }

    destroy_queue(&fields);
}

void parse_query(char* const query, dict_t* const dict) {
    if (!query)
        return;

    queue_t* q  = create_queue();
    char* param = strtok(query, "&");

    while (param) {
        enqueue(q, param);
        param = strtok(NULL, "&");
    }

    while (!queue_is_empty(q)) {
        dequeue(q, &param);

        char* key   = strtok(param, "=");
        char* value = strtok(NULL, "");

        DICT_SET_STRING(dict, key, value);
        free(param);
    }

    destroy_queue(&q);
}

void write_dict_entry(const dict_entry_t* const entry) {
    size_t key_len    = strlen(entry->key);
    size_t value_len  = strlen(entry->value);
    size_t total_size = key_len + value_len + 3;

    write(pipefd[WRITE_PIPE], &total_size, sizeof(size_t));
    write(pipefd[WRITE_PIPE], entry->key, key_len);
    write(pipefd[WRITE_PIPE], ": ", 2);
    write(pipefd[WRITE_PIPE], entry->value, value_len);
    write(pipefd[WRITE_PIPE], "\n", 1);
}
