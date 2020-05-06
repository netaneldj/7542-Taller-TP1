#ifndef SERVER_H
#define SERVER_H
#include "common_dbusmessage.h"
#include "common_socket.h"

typedef struct server {
    socket_t* socket;
} server_t;

/**
 * Constructor
 */
server_t* server_create();

/**
 * Destructor
 */
int server_destroy(server_t* self);

/**
 * Inicia el servidor en el puerto `service`, escucha un
 * *hola mundo* del cliente y luego le responde hola mundo también.
 **/
int server_run(server_t* self, char* service);

/**
 * Recibe el mensaje
 */
int server_recv_message(socket_t* skt);

/**
 * Envía el mensaje
 */
int server_send_message(socket_t* skt, char* msg, size_t len);

#endif // SERVER_H
