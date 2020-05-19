
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include "client_tda.h"
#include "common_dbusmessage.h"

#define BUFFER_SIZE 32
#define RECV_MSG_SIZE 14

#define SUCCESS 0
#define ERROR 1

/* ******************************************************************
 *                DEFINICION FUNCIONES AUXILIARES
 * *****************************************************************/

static void process_text_file(client_t* self, FILE* text_file);
static void process_text_line(client_t* self, vector_t* temp, int msgId);
static void client_comm(client_t* self, char* protocol, dbusmessage_t* msg);

/* ******************************************************************
 *                IMPLEMENTACION
 * *****************************************************************/

void client_create(client_t* self) {
	socket_create(&self->socket);
}

void client_destroy(client_t* self) {
    socket_destroy(&self->socket);
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

int client_send(client_t* self, char* protocol, size_t len) {
    return socket_send_message(&self->socket,protocol,len);
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
<<<<<<< HEAD
				char line[vector_obtener_cantidad(temp)];
				for (int j=0;j<vector_obtener_cantidad(temp);j++) {
					vector_obtener(temp,j,&line[j]);
				}
				msg = dbusmessage_create();
				dbusmessage_set_id(msg,msgId);
				protocolo = dbusmessage_client_get_protocol(msg,line);
				client_send(self,protocolo,dbusmessage_client_get_len_protocol(msg));
			    printf("%s\n",client_recv(self));
				dbusmessage_destroy(msg);
				vector_destruir(temp);
				temp = vector_crear(BUFFER_SIZE);
=======
				process_text_line(self, &temp, msgId);
				vector_destruir(&temp);
				vector_crear(&temp,BUFFER_SIZE);
>>>>>>> branch 'master' of https://github.com/netaneldj/tp1.git
				msgId++;
				continue;
			}
			vector_agregar(&temp,buffer[i]);
		}
	}
}

static void process_text_line(client_t* self, vector_t* temp, int msgId){
	char* protocolo = NULL;
	dbusmessage_t msg;

	char line[vector_obtener_cantidad(temp)];
	memset(line, 0, vector_obtener_cantidad(temp)*sizeof(char));
	for (int j=0; j<vector_obtener_cantidad(temp); j++) {
		vector_obtener(temp,j,&line[j]);
	}
	dbusmessage_create(&msg);
	dbusmessage_set_id(&msg,msgId);
	protocolo = dbusmessage_client_get_protocol(&msg,line);
	client_comm(self,protocolo,&msg);
	dbusmessage_destroy(&msg);
}

static void client_comm(client_t* self, char* protocol, dbusmessage_t* msg){
	char response[RECV_MSG_SIZE];

	client_send(self,protocol,dbusmessage_client_get_len_protocol(msg));
	client_recv(self, response);
    printf("%s\n",response);
}
