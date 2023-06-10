#include "server.h"

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "utils.h"

const server_t create_default_server(const int port) {
    return create_server(AF_INET, SOCK_STREAM, 0, INADDR_ANY, port, 32);
}
const server_t create_server(
    const int domain,
    const int sock_type,
    const int protocol,
    const unsigned long interface,
    const int port,
    const int backlog) {
    server_t server;

    server.address.sin_family      = domain;
    server.address.sin_port        = htons(port);
    server.address.sin_addr.s_addr = htonl(interface);

    server.sockfd = socket(domain, sock_type, protocol);
    if (server.sockfd < 0)
        err_n_die("Cannot open socket\n");

    if (bind(server.sockfd, (struct sockaddr*)&server.address, sizeof(server.address)) < 0)
        err_n_die("Cannot bind socket\n");

    if (listen(server.sockfd, backlog) < 0)
        err_n_die("Cannot start listening\n");

    return server;
}

void start_server(const server_t* const server) {
    char buffer[1024 * 128];
    char* hello     = "HTTP/1.1 200 OK\nGMT\nContent-Type: text/html\nConnection: Closed\n\n<html><body><h1>ma-ta</h1></body></html>";
    int addr_length = sizeof(server->address);
    int new_sockfd;

    while (1) {
        printf("\n\nWaiting for connection\n");

        new_sockfd = accept(server->sockfd, (struct sockaddr*)&server->address, (socklen_t*)&addr_length);
        if (new_sockfd < 0)
            err_n_die("Failed to open client connection\n");

        read(new_sockfd, buffer, sizeof(buffer));
        printf("%s\n", buffer);
        write(new_sockfd, hello, strlen(hello));
        close(new_sockfd);
    }
}
