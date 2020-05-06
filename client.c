#include <stdio.h>

#include "client_tda.h"

#define ARGS_HOST 1
#define ARGS_PORT 2
#define ARGS_FILE 3

#define SUCCESS 0
#define ERROR 1

int main(int argc, char** argv) {
    FILE* input;
	if (argc < 3) {
        printf("Uso: ./client <host> <puerto>");
        return ERROR;
    }
    if (argc > 3) {
        input = fopen(argv[3], "r");
    } else {
        input = stdin;
    }
    if (!input) {
        return ERROR;
    } else {
		const char* host = argv[ARGS_HOST];
		const char* service = argv[ARGS_PORT];
		client_t* client = client_create();
		client_run(client, host, service, input);
		client_destroy(client);
		if (input != stdin)
			fclose(input);
		return SUCCESS;
    }
}
