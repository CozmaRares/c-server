#ifndef __SERVER_H__
#define __SERVER_H__

#include <netinet/in.h>
#include <stdint.h>

#include "../utils/ds/dict.h"
#include "http.h"

typedef struct {
    struct sockaddr_in address;
    int sockfd;
    dict_t* route_handlers;
} server_t;

typedef http_response_t (*route_handler_t)(http_request_t* const);

server_t create_default_server(const uint16_t port);
server_t create_server(
    const sa_family_t domain,
    const uint16_t sock_type,
    const int protocol,
    const uint32_t interface,
    const uint16_t port);

void register_server_route(server_t* const server, const http_method_t method, const char* const url, route_handler_t handler);
void destroy_server(server_t* const server);
void start_server(server_t* const server);

#endif  // __SERVER_H__
