#include "server.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "http.h"
#include "utils.h"

server_t create_default_server(const int port) {
    return create_server(AF_INET, SOCK_STREAM, 0, INADDR_ANY, port, 32);
}
server_t create_server(
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

void* stop_server(void* arg) {
    server_t* server = (server_t*)arg;
    char c;

    while (1) {
        scanf("%c", &c);
        if (c == 'q')
            break;
    }
    printf("Exiting...\n");
    shutdown(server->sockfd, SHUT_RDWR);
    close(server->sockfd);
    exit(0);
    return NULL;
}

void print_ip(server_t* server) {
    char ipAddress[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server->address.sin_addr), ipAddress, INET_ADDRSTRLEN);
    printf("Server listening on: %s:%u\n", ipAddress, ntohs(server->address.sin_port));
}

void start_server(server_t* server) {
    char buffer[1024 * 128];
    char* hello     = "HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: Closed\n\n<html><body><h1>ma-ta</h1></body></html>";
    int addr_length = sizeof(server->address);
    int new_sockfd;

    pthread_t t;
    pthread_create(&t, NULL, stop_server, server);
    print_ip(server);

    while (1) {
        printf("\n\n====Waiting for connection====\n");

        new_sockfd = accept(server->sockfd, (struct sockaddr*)&server->address, (socklen_t*)&addr_length);
        if (new_sockfd < 0)
            err_n_die("Failed to open client connection\n");

        read(new_sockfd, buffer, sizeof(buffer));
        printf("%s\n", buffer);

        http_request_t req;
        if (!create_http_request(buffer, &req)) {
            printf("METHOD: %d\nURI: %s\nVERSION: %lf\n", req.method, req.uri, req.version);
            write(new_sockfd, hello, strlen(hello));
        } else {
            close(new_sockfd);
            exit(1);
        }

        close(new_sockfd);
    }
}
