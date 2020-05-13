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
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/
static bool socket_addrinfo(socket_t *self, char *host, char* service);

/* ******************************************************************
 *                IMPLEMENTACION
 * *****************************************************************/

void socket_create(socket_t* self) {
	self->skt = -1;
}

void socket_destroy(socket_t* self) {
	self->skt = -1;
}

bool socket_connect(socket_t* self, char* host, char* service) {
	bool are_we_connected = false;

	if (!socket_addrinfo(self,host,service)) {
		socket_destroy(self);
		return are_we_connected;
	}

   	for (self->ptr = self->addrinfo; self->ptr!=NULL &&
   		 !are_we_connected; self->ptr=self->ptr->ai_next) {
   		self->skt = socket(self->ptr->ai_family, self->ptr->ai_socktype,
   						   self->ptr->ai_protocol);
      	if (self->skt == -1) {
      		printf("Error: %s\n", strerror(errno));
      	} else {
      		self->s = connect(self->skt, self->ptr->ai_addr, self->ptr->ai_addrlen);
      		if (self->s == -1) {
			printf("Error: %s\n", strerror(errno));
			close(self->skt);
      		}
      		are_we_connected = (self->s != -1);
      }
    }
   	freeaddrinfo(self->addrinfo);
	if (are_we_connected == false) {
		return false;
	}
	return true;
}

int socket_close(socket_t* self) {
	shutdown(self->skt, 2);
	close(self->skt);
	return 0;
}

int socket_send_message(socket_t* self, char *buf, size_t size) {
    /**
     * MSG_NOSIGNAL para evitar que salte una señal en caso
     * de que el socket se haya cerrado
     */
	int sent = 0;
	int s = 0;
	bool is_the_socket_valid = true;

	while (sent < size && is_the_socket_valid) {
		s = send(self->skt, buf+sent, size-sent, MSG_NOSIGNAL);
		if (s <= 0) {
			is_the_socket_valid = false;
		} else {
			sent += s;
		}
	}
	return sent;
}

int socket_recv_message(socket_t* self, char* buf, size_t size) {
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

bool socket_bind(socket_t* self, char* port) {
	bool are_we_binded = false;

	if (!socket_addrinfo(self,NULL,port)) {
		socket_destroy(self);
		return are_we_binded;
	}

   	for (self->ptr = self->addrinfo; self->ptr!=NULL &&
   		 !are_we_binded; self->ptr=self->ptr->ai_next) {
   		self->skt = socket(self->ptr->ai_family, self->ptr->ai_socktype,
   						   self->ptr->ai_protocol);
      	if (self->skt == -1) {
      		printf("Error: %s\n", strerror(errno));
      	} else {
      		self->s = bind(self->skt, self->addrinfo->ai_addr,
      					   self->addrinfo->ai_addrlen);
      		if (self->s == -1) {
			printf("Error: %s\n", strerror(errno));
			close(self->skt);
      		}
      		are_we_binded = (self->s != -1);
      }
    }
   	freeaddrinfo(self->addrinfo);
	if (are_we_binded == false) {
		return false;
	}
	return true;
}

bool socket_listen(socket_t* self) {
    self->s = listen(self->skt, ACCEPT_QUEUE_LEN);
    if (self->s == -1) {
        socket_destroy(self);
        return false;
    }
    return true;
}

int socket_accept(socket_t* self, socket_t* skt_c) {
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
static bool socket_addrinfo(socket_t* self, char *host, char* service){
	struct addrinfo hints;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	self->s = getaddrinfo(host, service, &hints, &self->addrinfo);
   	if (self->s != 0) {
      		printf("Error in getaddrinfo: %s\n", gai_strerror(self->skt));
      		return false;
   	}
   	return true;
}
