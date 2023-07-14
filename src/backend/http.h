#ifndef __HTTP_H__
#define __HTTP_H__

#include "../utils/ds/dict.h"

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
    char* url;
    double version;
    dict_t* headers;
    dict_t* query;
    char* body;
} http_request_t;

// Taken from https://developer.mozilla.org/en-US/docs/Web/HTTP/Status
// only included what I thought was a must have
typedef enum {
    // success
    OK              = 200,
    CREATED         = 201,
    NO_CONTENT      = 204,
    PARTIAL_CONTENT = 206,

    // redirects
    MOVED_PERMANENTLY  = 301,
    FOUND              = 302,
    TEMPORARY_REDIRECT = 307,
    PERMANENT_REDIRECT = 308,

    // client error
    BAD_REQUEST = 400,
    NOT_FOUND   = 404,

    // server error
    INTERNAL_SERVER_ERROR      = 500,
    NOT_IMPLEMENTED            = 501,
    HTTP_VERSION_NOT_SUPPORTED = 505,

} http_status_code_t;

typedef struct {
    double version;
    http_status_code_t status;
    dict_t* headers;
    char* body;
} http_response_t;

http_status_code_t create_http_request(char* const request, http_request_t* const dest);
void destroy_http_request(http_request_t* const req);

http_response_t create_http_response();
void destroy_http_response(http_response_t* const response);
char* http_request_to_string(const http_response_t* const response);

#endif  // __HTTP_H__
