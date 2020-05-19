
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
	socket_create(&self->socket);
}

void server_destroy(server_t* self) {
    socket_destroy(&self->socket);
}

int server_run(server_t* self, char* service) {
	socket_t skt_a;

	if (!socket_bind(&self->socket, service)) {
        printf("No se pudo tomar el puerto");
        socket_close(&self->socket);
        return 1;
    }
	if (!socket_listen(&self->socket)) {
        printf("No se pudo escuchar el puerto");
        socket_close(&self->socket);
        return 1;
    }

	socket_create(&skt_a);
    if (socket_accept(&self->socket, &skt_a)) {
        printf("No se pudo aceptar el cliente");
        return 1;
    }

    server_recv_message(&skt_a);

    socket_close(&skt_a);
    socket_close(&self->socket);
    return 0;
}

int server_send_message(socket_t* skt, char* msg, size_t len) {
	return socket_send_message(skt,msg,len);
}

int server_recv_message(socket_t* skt){
<<<<<<< HEAD
	dbusmessage_t* msg;
	char** args;
	char hex[7] = "";
	char answer[9] = "";
=======
	dbusmessage_t msg;
	char** args = NULL;
	int id = -1;
	int buffer2Size = 0;
	char hex[11] = "";
	char answer[15] = "";
>>>>>>> branch 'master' of https://github.com/netaneldj/tp1.git
	char buffer1[BUFFER_SIZE];
	int lHeader, lBody, lPadding, received, i;

    do{
    	received = socket_recv_message(skt,buffer1,BUFFER_SIZE);
    	lBody = get_protocol_int(buffer1,4,8);
    	lHeader = get_protocol_int(buffer1,12,16);
    	lPadding = get_padding(lHeader);

<<<<<<< HEAD
    	if (lHeader==0) return -1;
=======
    	dbusmessage_create(&msg);
>>>>>>> branch 'master' of https://github.com/netaneldj/tp1.git

    	buffer2Size = lHeader+lPadding+lBody-BUFFER_SIZE;

    	char buffer2[buffer2Size];
    	char protocol[lHeader+lPadding+lBody];

    	received = socket_recv_message(skt,buffer2,buffer2Size);

    	for(i=0; i<BUFFER_SIZE; i++){
    		protocol[i] = buffer1[i];
    	}

    	for(int j=0; j<buffer2Size; j++){
    		protocol[i] = buffer2[j];
    		i++;
    	}

<<<<<<< HEAD
    	dbusmessage_server_set_message(msg,protocol,lHeader+lPadding+lBody);

		sprintf(hex,"%.4x",(int)dbusmessage_get_id(msg));
		printf("* Id: %s\n",hex);
		printf("* Destino: %s\n",dbusmessage_server_get_destination(msg));
		printf("* Path: %s\n",dbusmessage_server_get_path(msg));
		printf("* Interfaz: %s\n",dbusmessage_server_get_interface(msg));
		printf("* Metodo: %s\n",dbusmessage_server_get_method(msg));
		if (dbusmessage_server_get_cant_args(msg)>0) {
			printf("* Parámetros:\n");
			args = dbusmessage_server_get_args(msg);
			for(int i=0;i<dbusmessage_server_get_cant_args(msg);i++){
				printf("    * %s\n",args[i]);
			}
		strcpy(answer,hex);
		strcat(answer,RESPONSE);
		/*dbusmessage_destroy(msg);*/
=======
    	dbusmessage_server_set_message(&msg,protocol,lHeader+lPadding+lBody);
    	if (id>=(int)dbusmessage_get_id(&msg)) return 1;
    	id = (int)dbusmessage_get_id(&msg);

		snprintf(hex,sizeof(hex),"0x%.8x",(int)dbusmessage_get_id(&msg));
		printf("* Id: %s\n",hex);
		printf("* Destino: %s\n",dbusmessage_server_get_destination(&msg));
		printf("* Ruta: %s\n",dbusmessage_server_get_path(&msg));
		printf("* Interfaz: %s\n",dbusmessage_server_get_interface(&msg));
		printf("* Metodo: %s\n",dbusmessage_server_get_method(&msg));
		if (dbusmessage_server_get_cant_args(&msg)>0) {
			printf("* Parámetros:\n");
			args = dbusmessage_server_get_args(&msg);
			for(int i=0; i<dbusmessage_server_get_cant_args(&msg); i++){
				printf("    * %s\n",args[i]);
			}
		}
		snprintf(answer,sizeof(answer),"%s%s",hex,RESPONSE);
>>>>>>> branch 'master' of https://github.com/netaneldj/tp1.git
		server_send_message(skt,answer, strlen(answer));
    } while (received>0);
    return 0;
}
