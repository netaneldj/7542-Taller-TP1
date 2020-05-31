#ifndef SERVER_H
#define SERVER_H

#include "common_dbusmessage.h"
#include "common_socket.h"

typedef struct server {
    socket_t skt;
    socket_t skt_a;
    dbusmessage_t dbus;
} server_t;

/**
 * Constructor
 */
void server_create(server_t* self);

/**
 * Destructor
 */
void server_destroy(server_t* self);

/**
 * Inicia el servidor en el puerto `service`, escucha un
 * *hola mundo* del cliente y luego le responde hola mundo también.
 **/
int server_run(server_t* self, char* service);

/**
 * Recibe el mensaje
 */
int server_process(server_t* self);

/**
 * Recibe el mensaje
 */
int server_recv(server_t* self);

/**
 * Envía el mensaje
 */
int server_send(server_t* self);

#endif // SERVER_H
