#ifndef __SERVER_H__
#define __SERVER_H__

#include <netinet/in.h>

typedef struct {
    struct sockaddr_in address;
    int sockfd;
} server_t;

server_t create_default_server(const int port);
server_t create_server(
    const int domain,
    const int sock_type,
    const int protocol,
    const unsigned long interface,
    const int port,
    const int backlog);

void start_server(server_t* server);

#endif  // __SERVER_H__
