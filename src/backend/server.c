#include "server.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../utils/utils.h"
#include "http.h"

#define MAX_PATH 4096

typedef struct stat stat_t;
typedef struct dirent dirent_t;

void print_ip(const server_t* const server);
void handle_request(const server_t* const server, http_request_t* const req, int new_sockfd);
char* read_file(const char* const path);
void load_page(char* const path, http_response_t* const response);
void send_file(const char* const path, http_response_t* const response);
// void handle_api_request(http_request_t* const req, http_response_t response);
char* get_mime_type(const char* const extension);

server_t create_default_server(const int port) {
    return create_server(AF_INET, SOCK_STREAM, 0, INADDR_ANY, port);
}
server_t create_server(
    const int domain,
    const int sock_type,
    const int protocol,
    const unsigned long interface,
    const int port) {
    server_t server;

    server.address.sin_family      = domain;
    server.address.sin_port        = htons(port);
    server.address.sin_addr.s_addr = htonl(interface);

    server.sockfd = socket(domain, sock_type, protocol);
    if (server.sockfd < 0)
        err_n_die("Cannot open socket\n");

    if (bind(server.sockfd, (struct sockaddr*)&server.address, sizeof(server.address)) < 0)
        err_n_die("Cannot bind socket\n");

    server.route_handlers = create_default_dict();

    return server;
}

void destroy_server(server_t* const server) {
    destroy_dict(&server->route_handlers);
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

void start_server(server_t* const server) {
    if (listen(server->sockfd, 32) < 0)
        err_n_die("Cannot start server\n");

    char request[128 * 1024];
    int addr_length = sizeof(server->address);
    int new_sockfd;

    pthread_t t;
    pthread_create(&t, NULL, stop_server, server);
    print_ip(server);

    while (1) {
        printf("\n\n==== Waiting for connection ====\n");

        new_sockfd = accept(server->sockfd, (struct sockaddr*)&server->address, (socklen_t*)&addr_length);
        if (new_sockfd < 0)
            err_n_die("Failed to open client connection\n");

        ssize_t bytes = read(new_sockfd, request, sizeof(request));

        if (bytes < 0)
            err_n_die("Failed to read request\n");

        request[bytes] = '\0';
        printf("%s\n", request);

        http_request_t req;
        char* err = create_http_request(request, &req);
        if (err) {
            fprintf(stderr, "%s\n", err);
            close(new_sockfd);
            exit(1);
        }

        handle_request(server, &req, new_sockfd);
        destroy_http_request(&req);
        close(new_sockfd);
    }
}

void print_ip(const server_t* const server) {
    char ipAddress[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server->address.sin_addr), ipAddress, INET_ADDRSTRLEN);
    printf("Server listening on: %s:%u\n", ipAddress, ntohs(server->address.sin_port));
}

void handle_request(const server_t* const server, http_request_t* const req, int new_sockfd) {
    char path[MAX_PATH] = { 0 };
    char* response;
    http_response_t res = create_http_response();

    if (strstr(req->uri, "..")) {
        res.status = MOVED_PERMANENTLY;
        dict_set(res.headers, "Location", "/404.html");
        goto _send_response;
    }

    // TODO: if server.route_handlers contains url, execute api call if url starts with '/api', or get data in dict
    // else ....
    if (req->method != GET) {
        res.status = NOT_FOUND;
        goto _send_response;
    }

    sprintf(path, "pages%s", req->uri);
    if (strchr(req->uri, '.') != NULL)
        send_file(path, &res);
    else
        load_page(path, &res);

_send_response:
    response = http_request_to_string(&res);
    printf("\n%s", response);
    write(new_sockfd, response, strlen(response));
    destroy_http_response(&res);
    free(response);
}

void load_page(char* const path, http_response_t* const response) {
    char* index = path[strlen(path) - 1] == '/' ? "index.html" : "/index.html";
    strcat(path, index);
    send_file(path, response);
}

void send_file(const char* const path, http_response_t* const response) {
    char* contents = read_file(path);
    if (contents == NULL) {
        response->status = NOT_FOUND;
        return;
    }

    char* extension = strrchr(path, '.');
    char* mime_type = get_mime_type(extension);

    response->status = OK;
    dict_set(response->headers, "Content-Type", mime_type);
    response->body = contents;
}

char* read_file(const char* const path) {
    stat_t status;

    if (stat(path, &status) == -1 || !S_ISREG(status.st_mode))
        return NULL;

    int file_fd = open(path, O_RDONLY);

    if (file_fd == -1)
        return NULL;

    char* contents;
    MALLOC(char, contents, status.st_size + 1);
    read(file_fd, contents, status.st_size);
    contents[status.st_size] = '\0';

    return contents;
}

char* get_mime_type(const char* const extension) {
    struct {
        char* ext;
        char* mime_type;
    } pairs[] = {
        { .ext = ".html", .mime_type = "text/html" },
        { .ext = ".css", .mime_type = "text/css" },
        { .ext = ".js", .mime_type = "text/javascript" },
        { .ext = ".ico", .mime_type = "image/vnd.microsoft.icon" },
        { .ext = ".json", .mime_type = "application/json" },
        { .ext = ".mjs", .mime_type = "text/javascript" },
        { .ext = ".gif", .mime_type = "image/gif" },
        { .ext = ".svg", .mime_type = "image/svg+xml" },
        { .ext = ".jpg", .mime_type = "image/jpeg" },
        { .ext = ".jpeg", .mime_type = "image/jpeg" },
        { .ext = ".png", .mime_type = "image/png" },
        { .ext = ".csv", .mime_type = "text/csv" },
        { .ext = ".epub", .mime_type = "application/epub+zip" },
        { .ext = ".mp3", .mime_type = "audio/mpeg" },
        { .ext = ".mp4", .mime_type = "video/mp4" },
        { .ext = ".mpeg", .mime_type = "video/mpeg" },
        { .ext = ".pdf", .mime_type = "application/pdf" },
        { .ext = ".ttf", .mime_type = "font/ttf" },
        { .ext = ".txt", .mime_type = "text/plain" },
        { .ext = ".wav", .mime_type = "audio/wav" },
        { .ext = ".weba", .mime_type = "audio/webm" },
        { .ext = ".webm", .mime_type = "video/webm" },
        { .ext = ".webp", .mime_type = "image/webp" },
        { .ext = ".woff", .mime_type = "font/woff" },
        { .ext = ".woff2", .mime_type = "font/woff2" },
    };
    int size = sizeof(pairs) / sizeof(pairs[0]);

    for (int i = 0; i < size; i++)
        if (strcmp(extension, pairs[i].ext) == 0)
            return pairs[i].mime_type;

    // treat unknown extensions as plain text
    return "text/plain";
}
