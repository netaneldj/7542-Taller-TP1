
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include "client_tda.h"
#include "common_dbusmessage.h"
#include "common_dynamicvector.h"

#define BUFFER_SIZE 32
#define RECV_MSG_SIZE 14

#define SUCCESS 0
#define ERROR 1

/* ******************************************************************
 *                DEFINICION FUNCIONES AUXILIARES
 * *****************************************************************/

static void process_text_file(client_t* self, FILE* text_file);
static void process_msg(client_t* self, vector_t* temp, int msgId);

/* ******************************************************************
 *                IMPLEMENTACION
 * *****************************************************************/

void client_create(client_t* self) {
	socket_create(&self->socket);
	dbusmessage_create(&self->dbus);
}

void client_destroy(client_t* self) {
    socket_destroy(&self->socket);
	dbusmessage_destroy(&self->dbus);
}

int client_run(client_t* self, char* host, char* service, FILE *text_file) {
    if (!socket_connect(&self->socket, host, service)) {
        printf("No se pudo conectar al servidor");
        return 1;
    }
    process_text_file(self, text_file);
    socket_close(&self->socket);
    return 0;
}

int client_send(client_t* self, dbusmessage_t* dbus, char* msg) {
    return dbusmessage_client_send(dbus, &self->socket, msg);
}

void client_recv(client_t* self, char* response) {
	socket_recv_message(&self->socket,response,RECV_MSG_SIZE);
}

/* ******************************************************************
 *                IMPLEMENTACIONES AUXILIARES
 * *****************************************************************/


static void process_text_file(client_t* self, FILE* text_file) {
	char buffer[BUFFER_SIZE] = "";
	int bytes = 0;
	int msgId = 1;
	vector_t temp;

	vector_crear(&temp, BUFFER_SIZE);
	while (!feof(text_file)){
		bytes = fread(buffer, 1, BUFFER_SIZE, text_file);
		for (int i=0; i<bytes; i++){
			if(buffer[i]=='\n'){
				vector_agregar(&temp,'\0');
				process_msg(self, &temp, msgId);
				vector_destruir(&temp);
				vector_crear(&temp,BUFFER_SIZE);
				msgId++;
				continue;
			}
			vector_agregar(&temp,buffer[i]);
		}
	}
	vector_destruir(&temp);
}

static void process_msg(client_t* self, vector_t* temp, int msgId){
	char response[RECV_MSG_SIZE];
	char line[vector_obtener_cantidad(temp)];
	vector_t header, body;

	memset(line, 0, vector_obtener_cantidad(temp)*sizeof(char));
	for (int j=0; j<vector_obtener_cantidad(temp); j++) {
		vector_obtener(temp,j,&line[j]);
	}
	vector_crear(&header,0);
	vector_crear(&body,0);
	dbusmessage_set_id(&self->dbus,msgId);
	dbusmessage_set_header(&self->dbus, &header);
	dbusmessage_set_body(&self->dbus, &body);
	client_send(self,&self->dbus, line);
	client_recv(self, response);
    printf("%s\n",response);
    vector_destruir(&header);
    vector_destruir(&body);
}
