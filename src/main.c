#include <stdio.h>
#include <stdlib.h>

#include "backend/server.h"

int main(int argc, char** argv) {
    int port = 8080;

    if (argc == 2) {
        int is_number = 1;
        for (int i = 0; argv[1][i] && is_number; i++)
            is_number = argv[1][i] >= '0' && argv[1][i] <= '9';

        if (is_number)
            port = atoi(argv[1]);
    }

    server_t server = create_default_server(port);
    start_server(&server);
}
