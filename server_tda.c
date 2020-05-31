
#include <unistd.h>
#include <arpa/inet.h> // inet_ntop
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include "server_tda.h"
#include "common_socket.h"
#include "common_dbusmessage.h"
#include "common_dynamicvector.h"

#define BUFFER_SIZE 16
#define UINT32 4

char RESPONSE[] = ": OK";

/* ******************************************************************
 *                IMPLEMENTACION
 * *****************************************************************/

void server_create(server_t* self) {
	socket_create(&self->skt);
	socket_create(&self->skt_a);
	dbusmessage_create(&self->dbus);
}

void server_destroy(server_t* self) {
    socket_destroy(&self->skt);
    dbusmessage_destroy(&self->dbus);
}

int server_run(server_t* self, char* service) {
	if (!socket_bind(&self->skt, service)) {
        printf("No se pudo tomar el puerto");
        socket_close(&self->skt);
        return 1;
    }
	if (!socket_listen(&self->skt)) {
        printf("No se pudo escuchar el puerto");
        socket_close(&self->skt);
        return 1;
    }

    if (socket_accept(&self->skt, &self->skt_a)) {
        printf("No se pudo aceptar el cliente");
        return 1;
    }

    server_process(self);

    socket_close(&self->skt_a);
    socket_close(&self->skt);
    return 0;
}

int server_process(server_t* self) {
	while(server_recv(self)>0) {
		server_send(self);
	}
	return 0;
}

int server_send(server_t* self) {
	char answer[15] = "";
	snprintf(answer,sizeof(answer),"0x%.8x%s",(int)dbusmessage_get_id(&self->dbus),RESPONSE);
	return socket_send_message(&self->skt_a,answer,strlen(answer));
}

int server_recv(server_t* self){
    return dbusmessage_server_recv(&self->dbus,&self->skt_a);
}
