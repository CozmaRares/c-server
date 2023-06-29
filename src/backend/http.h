#ifndef __HTTP_H__
#define __HTTP_H__

#include "../utils/utils.h"

typedef enum {
    GET,
    PUT,
    HEAD,
    POST,
    PATCH,
    TRACE,
    DELETE,
    CONNECT,
    OPTIONS,
} http_method_t;

typedef struct {
    http_method_t method;
    char* uri;
    double version;
    dict_t* headers;
} http_request_t;

int create_http_request(char* request, http_request_t* const dest);
void free_http_request(http_request_t* const req);

#endif  // __HTTP_H__
