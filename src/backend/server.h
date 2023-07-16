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


/**
 * Creates a default server with the specified port.
 * Default server parameters:
 *   - domain: AF_INET
 *   - sock_type: SOCK_STREAM
 *   - protocol: 0
 *   - interface : INADDR_ANY
 *
 * @param port The port number for the server to listen on.
 * @return The created server_t structure.
 */
server_t create_default_server(const uint16_t port);

/**
 * Creates a server with the specified configuration.
 *
 * @param domain The socket domain (e.g., AF_INET or AF_INET6).
 * @param sock_type The socket type (e.g., SOCK_STREAM or SOCK_DGRAM).
 * @param protocol The protocol for the socket (e.g., IPPROTO_TCP or IPPROTO_UDP).
 * @param interface The network interface to bind the server socket to (e.g., INADDR_ANY or specific IP address).
 * @param port The port number for the server to listen on.
 * @return The created server_t structure.
 */
server_t create_server(
    const sa_family_t domain,
    const uint16_t sock_type,
    const int protocol,
    const uint32_t interface,
    const uint16_t port);

/**
 * Registers a route handler for the specified HTTP method and URL on the server.
 *
 * @param server A pointer to the server_t structure.
 * @param method The HTTP method associated with the route (e.g., GET, POST).
 * @param url The URL pattern for the route.
 * @param handler The route handler function to be registered.
 */
void register_server_route(server_t* const server, const http_method_t method, const char* const url, route_handler_t handler);

/**
 * Frees all associated memory with the server.
 *
 * @param server A pointer to the server_t structure to be destroyed.
 */
void destroy_server(server_t* const server);

/**
 * Starts the server, allowing it to accept incoming connections and handle requests.
 *
 * @param server A pointer to the server_t structure.
 */
void start_server(server_t* const server);

#endif  // __SERVER_H__
