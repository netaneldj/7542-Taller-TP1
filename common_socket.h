#ifndef COMMON_SOCKET_H_
#define COMMON_SOCKET_H_

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "common_dynamicvector.h"

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct socket {
	int skt;
} socket_t;

/* ******************************************************************
 *                    PRIMITIVAS DEL MENSAJE
 * *****************************************************************/

socket_t* socket_create();

void socket_destroy(socket_t *self);

bool socket_connect(socket_t *self, const char* host, const char* service);

int socket_close(socket_t* self);

int socket_send_message(socket_t *self, char *buf, size_t size);

int socket_recv_message(socket_t *self, char *buf, size_t size);

int socket_bind_listen(socket_t *self, char* port);

int socket_accept(socket_t *self, socket_t* skt_c);

#endif /* COMMON_SOCKET_H_ */
