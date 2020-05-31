#include <stdio.h>

#include "client_tda.h"

#define ARGS_HOST 1
#define ARGS_PORT 2
#define ARGS_FILE 3

#define SUCCESS 0
#define ERROR 1

int main(int argc, char** argv) {
    FILE* input;
	if (argc < 3 || argc > 4) {
        printf("Uso: ./client <host> <puerto> [<archivo de entrada>]");
        return ERROR;
    }
    if (argc == 4) {
        input = fopen(argv[3], "r");
    } else {
        input = stdin;
    }
    if (!input) {
        return ERROR;
    } else {
		char* host = argv[ARGS_HOST];
		char* service = argv[ARGS_PORT];
		client_t client;
		client_create(&client);
		client_run(&client, host, service, input);
		client_destroy(&client);
		if (input != stdin)
			fclose(input);
		return SUCCESS;
    }
}
