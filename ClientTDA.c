
#define _POSIX_C_SOURCE 201112L // Habilita getaddrinfo
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include "ClientTDA.h"
#include "DBusMessage.h"

#define BUFFER_SIZE 32
#define RECV_MSG_SIZE 12

#define SUCCESS 0
#define ERROR 1

/* ******************************************************************
 *                DEFINICION FUNCIONES AUXILIARES
 * *****************************************************************/

int process_text_file(client_t* self, FILE* text_file);

/* ******************************************************************
 *                IMPLEMENTACION
 * *****************************************************************/

client_t* client_create() {
	client_t* client = malloc(sizeof(client_t));
	if (client==NULL) return NULL;
	client->socket = socket_create();
	if (client->socket==NULL) return NULL;
    return client;
}

int client_destroy(client_t* self) {
    socket_destroy(self->socket);
    free(self);
    return 0;
}

int client_run(client_t* self, const char* host, const char* service, FILE *text_file) {

    if (!socket_connect(self->socket, host, service)) {
        printf("No se pudo conectar al servidor");
        return 1;
    }

    process_text_file(self, text_file);
    
    socket_close(self->socket);
    return 0;
}

int client_send(client_t* self, char* protocol, size_t len) {
    return socket_send_message(self->socket,protocol,len);
}

char* client_recv(client_t* self) {
	char buffer[RECV_MSG_SIZE];
	int received;
	char* msg;

	received = socket_recv_message(self->socket,buffer,RECV_MSG_SIZE);
	msg = malloc(received*sizeof(char));
	if (msg==NULL) return NULL;
	for(int i=0;i<received;i++){
		msg[i]=buffer[i];
	}
	return msg;
}

int process_text_file(client_t* self, FILE* text_file) {
	char buffer[BUFFER_SIZE];
	//char hex[7];
	char *protocolo;
	int bytes;
	int msgId;
	dbusmessage_t* msg;
	vector_t* temp;

	temp = vector_crear(BUFFER_SIZE);
	msgId = 1;
	fseek(text_file, 0, SEEK_SET);
	while (!feof(text_file)){
		bytes = fread(buffer, 1, BUFFER_SIZE, text_file);
		for (int i=0;i<bytes;i++){
			if(buffer[i]=='\n'){
				char line[vector_obtener_cantidad(temp)];
				for (int j=0;j<vector_obtener_cantidad(temp);j++) {
					vector_obtener(temp,j,&line[j]);
				}
				msg = dbusmessage_create();
				dbusmessage_set_id(msg,msgId);
				protocolo = dbusmessage_client_get_protocol(msg,line);
				client_send(self,protocolo,dbusmessage_client_get_len_protocol(msg));
			    /*sprintf(hex,"0x%.4x",(int)dbusmessage_get_id(msg));
			    printf("%s: %s\n",hex,client_recv(self));*/
				dbusmessage_destroy(msg);
				vector_destruir(temp);
				temp = vector_crear(BUFFER_SIZE);
				msgId++;
				continue;
			}
			vector_agregar(temp,buffer[i]);
		}
	}
	if(buffer!='\0'){
		char line[vector_obtener_cantidad(temp)];
		for (int j=0;j<vector_obtener_cantidad(temp);j++) {
			vector_obtener(temp,j,&line[j]);
		}
		msg = dbusmessage_create();
		dbusmessage_set_id(msg,msgId);
		protocolo = dbusmessage_client_get_protocol(msg,line);
		client_send(self,protocolo,dbusmessage_client_get_len_protocol(msg));
	    /*sprintf(hex,"0x%.4x",(int)dbusmessage_get_id(msg));
	    printf("%s: %s\n",hex,client_recv(self));*/
		dbusmessage_destroy(msg);
		vector_destruir(temp);
	}
	return 0;
}
