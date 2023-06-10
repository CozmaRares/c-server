#include <stdio.h>

#include "server.h"

int main() {
    const server_t server = create_default_server(8080);
    start_server(&server);
}
