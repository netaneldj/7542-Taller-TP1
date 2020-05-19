#include <stdio.h>

#include "server_tda.h"

#define ARGS_PORT 1

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Uso: ./server <puerto>");
    return 1;
    }
    char* service = argv[ARGS_PORT];
    server_t server;
    server_create(&server);
    server_run(&server, service);
    server_destroy(&server);
}
