#ifndef CLIENT_H
#define CLIENT_H

#include "common_socket.h"

typedef struct client {
    socket_t* socket;
} client_t;

/**
 * Constructor
 */
client_t* client_create();

/**
 * Destructor
 */
int client_destroy(client_t* self);

/**
 * Se conecta a un servidor con la dirección dada por <host> y <service>
 * Envía el mensaje leido por consola o por archivo al servidor y luego espera su respuesta.
 */
int client_run(client_t* self, const char* host, const char* service, FILE *text_file);

/**
 * Envía el mensaje "Hello client"
 */
char* client_recv(client_t* self);

/**
 * Envía el mensaje
 */
int client_send(client_t* self, char* protocol, size_t len);

#endif // CLIENT_H
