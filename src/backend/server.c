#include "server.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../utils/utils.h"
#include "http.h"

void print_ip(const server_t* const server);
void handle_request(http_request_t* const req, int sockfd);
char* read_file(const char* const path);

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

void start_server(server_t* server) {
    char buffer[1024 * 128];
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
        if (!create_http_request(buffer, &req))
            handle_request(&req, new_sockfd);
        else {
            close(new_sockfd);
            exit(1);
        }

        free_http_request(&req);
        close(new_sockfd);
    }
}

void print_ip(const server_t* const server) {
    char ipAddress[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server->address.sin_addr), ipAddress, INET_ADDRSTRLEN);
    printf("Server listening on: %s:%u\n", ipAddress, ntohs(server->address.sin_port));
}

void handle_request(http_request_t* const req, int sockfd) {
    char path[30000]     = { 0 };
    char response[30000] = { 0 };

    sprintf(path, "index.html");

    char* contents = read_file(path);

    if (contents == NULL)
        sprintf(response, "HTTP/1.1 500 Could not open file");
    else {
        sprintf(response, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n%s", contents);
        free(contents);
    }

    printf("\n%s\n\n", response);
    write(sockfd, response, strlen(response));
}

char* read_file(const char* const path) {
    int file_fd = open(path, O_RDONLY);

    if (file_fd == -1)
        return NULL;

    off_t file_size = lseek(file_fd, 0, SEEK_END);
    lseek(file_fd, 0, SEEK_SET);

    char* contents;
    MALLOC(char, contents, file_size + 1);
    read(file_fd, contents, file_size);
    contents[file_size] = '\0';

    return contents;
}
