#define _POSIX_C_SOURCE 201112L // Habilita getaddrinfo
#include <unistd.h>
#include <arpa/inet.h> // inet_ntop
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 32
#define ACCEPT_QUEUE_LEN 10

#include "common_socket.h"

/* ******************************************************************
 *                IMPLEMENTACION
 * *****************************************************************/

socket_t* socket_create() {
	socket_t* socket = malloc(sizeof(socket_t));
	if (socket==NULL) return NULL;
	socket->skt = -1;
	return socket;
}

void socket_destroy(socket_t *self) {
	self->skt = -1;
	free(self);
}

bool socket_connect(socket_t *self, const char* host, const char* service) {
	struct addrinfo hints, *serv_info;
	memset(&hints, 0, sizeof(struct addrinfo));
    // serv_info es una lista enlazada, igualmente me quedo con la primer opción
    int error = getaddrinfo(host, service, &hints, &serv_info);
    if (error) {
        return false;
    }
    self->skt = socket(serv_info->ai_family, serv_info->ai_socktype, serv_info->ai_protocol);
    connect(self->skt, serv_info->ai_addr, serv_info->ai_addrlen);

    // libero recursos de getaddrinfo
    freeaddrinfo(serv_info);
    return self->skt!=-1;
}

int socket_close(socket_t* self) {
	shutdown(self->skt, 2);
	close(self->skt);
	return 0;
}

int socket_send_message(socket_t *self, char *buf, size_t size) {
    /**
     * MSG_NOSIGNAL para evitar que salte una señal en caso
     * de que el socket se haya cerrado
     */
	int sent = 0;
	int s = 0;
	bool is_the_socket_valid = true;

	while (sent < size && is_the_socket_valid) {
		s = send(self->skt, buf+sent, size-sent, MSG_NOSIGNAL);
		if (s == 0) {
			is_the_socket_valid = false;
		} else if (s < 0) {
			is_the_socket_valid = false;
		} else {
			sent += s;
		}
	}

	if (is_the_socket_valid) {
		return sent;
	} else {
		return -1;
	}
}

int socket_recv_message(socket_t *self, char* buf, size_t size) {
	int received = 0;
	int s = 0;
	bool is_the_socket_valid = true;
	while (received < size && is_the_socket_valid) {
		s = recv(self->skt, buf+received, size-received, MSG_NOSIGNAL);
		if (s <= 0) {
			is_the_socket_valid = false;
			return -1;
		} else {
			received += s;
		}
	}

	return received;
}

int socket_bind_listen(socket_t *self, char* port) {
    // Hints son las opciones que configuro, server info posee los datos
    // devueltos por el sistema.

    struct addrinfo hints, *server_info;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    getaddrinfo(NULL, port, &hints, &server_info);
    self->skt = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

    // Así se puede reutilizar el puerto sin esperar timeout
    int val = 1;
    setsockopt(self->skt, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    bind(self->skt, server_info->ai_addr, server_info->ai_addrlen);

    freeaddrinfo(server_info);
    listen(self->skt, ACCEPT_QUEUE_LEN);

    return 0;
}

int socket_accept(socket_t *self, socket_t* skt_c) {
    char addressBuf[INET_ADDRSTRLEN];
    struct sockaddr_in address;
    socklen_t addressLength = (socklen_t) sizeof(address);

    int newSocket = accept(self->skt, (struct sockaddr *)&address,&addressLength);
    inet_ntop(AF_INET, &(address.sin_addr), addressBuf, INET_ADDRSTRLEN);
    skt_c->skt = newSocket;
    return 0;
}

/* ******************************************************************
 *                IMPLEMENTACIONES AUXILIARES
 * *****************************************************************/

