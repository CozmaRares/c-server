#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backend/http.h"
#include "backend/server.h"
#include "utils/utils.h"

http_response_t handler(__attribute__((unused)) http_request_t* const req) {
    http_response_t res = create_http_response();

    res.status = OK;
    res.body   = new_string("Hello, World!");
    printf("From server: %s\n", res.body);

    return res;
}

int main(int argc, char** argv) {
    int port = 8080;

    if (argc == 2) {
        bool is_number = true;
        for (int i = 0; argv[1][i] && is_number; i++)
            is_number = argv[1][i] >= '0' && argv[1][i] <= '9';

        if (is_number)
            port = atoi(argv[1]);
    }

    server_t server = create_default_server((uint16_t)port);

    register_server_route(&server, GET, "/server", handler);

    start_server(&server);
}
