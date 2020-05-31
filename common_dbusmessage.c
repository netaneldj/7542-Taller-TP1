#include "common_dbusmessage.h"

#include "common_dynamicvector.h"
#include "common_socket.h"
#include <stdlib.h>
#include <stdio.h>

#define UINT32 8
#define PARAMETERS 5
#define BUFFER_SIZE 32

/* ******************************************************************
 *                DEFINICION FUNCIONES AUXILIARES
 * *****************************************************************/
// Guarda el protocol en el mensaje
static int set_protocol(dbusmessage_t* self, char* msg, size_t len);
//Guarda espacio para un uint32 en el mensaje
static void set_protocol_init_uint32(vector_t* self);
//Escribe el numero en formato uint32
static void set_protocol_uint32(vector_t* self, int num);
//Escribe el numero en formato uint32 desde s hasta f
static void set_protocol_uint32_custom(vector_t* self, int num, int s, int f);
//Agrega los datos de la firma del mensaje
static void set_protocol_args(dbusmessage_t* self, char* param);
//Agrega las constantes del parametro al mensaje
static void set_protocol_padding(vector_t* self);
//Escribe el parametro en el mensaje(client)
static void set_protocol_param(vector_t* self,char* param);
//Establece las constantes de los parametrso del mensaje(client)
static void set_protocol_param_const(vector_t* self,int id, int cnt, char type);
//Agrega el encabezado del header(client)
static void set_protocol_header_constants(vector_t* self);
//Procesa los parametros del mensaje(client)
static void set_protocol_process_input(dbusmessage_t* self, char* input);
//Procesa el destino
static void set_protocol_process_destination(dbusmessage_t* self, char* destination);
//Procesa la ruta
static void set_protocol_process_path(dbusmessage_t* self, char* path);
//Procesa la interface
static void set_protocol_process_interface(dbusmessage_t* self, char* interface);
//Procesa el metodo
static void set_protocol_process_method(dbusmessage_t* self, char* method);
//Procesa los argumentos del metodo(client)
static void set_protocol_process_args(dbusmessage_t* self,char* args);
//Procesa los argumentos del metodo(client)
static void set_protocol_process_args(dbusmessage_t* self,char* param);
//Actualiza la longitud del header y del body del protocolo(client)
static void update_protocol_len(dbusmessage_t* self);
//Convierte de entero a uint32
static void int_to_uint32(long int num, char* res);
//Convierte de hexadecimal de 4 bytes a entero
static int uint32_to_int(char* uint32);
//Devuelve la cantidad de elementos existentes en el split
//de la cadena param con el delimitador delim(client)
static int get_cant_split(char* param);
//Escribe el mensaje unificado a retornar con el header y el body(client)
static int send_message(dbusmessage_t* self, socket_t* skt);
//Devuelve el valor decimal del entero de 4 bytes almacenado
//en el mensaje entre las posiciones start y finish(server)
static int get_msg_int(dbusmessage_t* self,int start, int finish);
//Procesa los parametros del mensaje y los guarda en sus
//respectivas variables dentro del mensaje(server)
static int process_msg_parameters(dbusmessage_t* self);
//Procesa los argumentos del metodo y los guarda en la lista
//de argumentos del mensaje(server)
static int process_msg_args(dbusmessage_t* self);

/* ******************************************************************
 *                IMPLEMENTACION
 * *****************************************************************/
void dbusmessage_create(dbusmessage_t* self){
	self->id = 0;
	self->msg = NULL;
	self->header = NULL;
	self->body = NULL;
	self->lHeader = 0;
	self->lBody = 0;
	self->lMsg = 0;
}

void dbusmessage_destroy(dbusmessage_t* self){
	self->id = 0;
	self->msg = NULL;
	self->header = NULL;
	self->body = NULL;
	self->lHeader = 0;
	self->lBody = 0;
	self->lMsg = 0;
}

void dbusmessage_set_id(dbusmessage_t* self, int id){
	self->id = id;
}

int dbusmessage_get_id(dbusmessage_t* self){
	return self->id;
}

void dbusmessage_set_header(dbusmessage_t* self, vector_t* header) {
	self->header = header;
}

vector_t* dbusmessage_get_header(dbusmessage_t* self) {
	return self->header;
}

void dbusmessage_set_body(dbusmessage_t* self, vector_t* body) {
	self->body = body;
}

vector_t* dbusmessage_get_body(dbusmessage_t* self) {
	return self->body;
}

int dbusmessage_client_send(dbusmessage_t* self, socket_t* skt, char* input){
	set_protocol_header_constants(self->header);
	set_protocol_init_uint32(self->header);
	set_protocol_uint32(self->header,dbusmessage_get_id(self));
	set_protocol_init_uint32(self->header);
	set_protocol_process_input(self,input);
	set_protocol_uint32_custom(self->header,
							   (int)dbusmessage_client_get_len_body(self),4,8);
	set_protocol_uint32_custom(self->header,
							   (int)dbusmessage_client_get_len_header(self),12,16);
	return send_message(self, skt);
}

//la suma de len header y len body no es la misma que len protocol
//porque ahi se cuenta el padding del ultimo parametro
size_t dbusmessage_client_get_len_header(dbusmessage_t* self){
	return self->lHeader;
}
size_t dbusmessage_client_get_len_body(dbusmessage_t* self){
	return self->lBody;
}

int dbusmessage_server_recv(dbusmessage_t* self, socket_t* skt) {
	int remain = 0;
	char buffer1[BUFFER_SIZE];
	int lHeader, lBody, lPadding, i;

	if (socket_recv_message(skt,buffer1,BUFFER_SIZE) <= 0) return 0;

	lBody = get_protocol_int(buffer1,4,8);
	lHeader = get_protocol_int(buffer1,12,16);
	lPadding = get_padding(lHeader);

	char protocol[lHeader+lPadding+lBody];
	for(i=0; i<BUFFER_SIZE; i++){
		protocol[i] = buffer1[i];
	}

	remain = lHeader+lPadding+lBody-BUFFER_SIZE;

	char buffer2[remain];

	if (socket_recv_message(skt,buffer2,remain) <= 0) return 0;

	for(int j=0; j<remain; j++){
		protocol[i] = buffer2[j];
		i++;
	}

	set_protocol(self,protocol,lHeader+lPadding+lBody);

    return 1;
}

/* ******************************************************************
 *                IMPLEMENTACIONES AUXILIARES
 * *****************************************************************/

static int set_protocol(dbusmessage_t* self, char* msg, size_t len){
	char hex[11] = "";

	self->msg = msg;
	self->lMsg = len;
	self->lBody = get_msg_int(self,4,8);
	self->id = get_msg_int(self,8, 12);
	self->lHeader = get_msg_int(self,12,16);

	snprintf(hex,sizeof(hex),"0x%.8x",(int)self->id);
	printf("* Id: %s\n",hex);

	process_msg_parameters(self);
	return 0;
}

static int process_msg_args(dbusmessage_t* self){
	int pos = self->lMsg-self->lBody;
	int i = 0;
	int len;

	while(pos<self->lMsg){
		len = get_msg_int(self,pos,pos+4);
		char arg[len+1];
		strncpy(arg,self->msg+pos+4,len+1);
		printf("    * %s\n",arg);
		i++;
		pos+=4+len+1;
	}
	printf("\n");
	return 0;
}

static int process_msg_parameters(dbusmessage_t* self){
	int pos = 16;
	int tipo, len, padding;
	char byte[3] = "";

	while (pos<self->lHeader) {
		snprintf(byte,sizeof(byte),"%02hhX",self->msg[pos]);
		tipo = (int)strtol(byte, NULL, 16);
		len = get_msg_int(self,pos+4,pos+8);
		padding = get_padding(pos+8+len+1);
		char param[len+1];
		if (tipo != 8) strncpy(param,self->msg+pos+8,len+1);
		switch(tipo){
		case 6:
			printf("* Destino: %s\n",param);
			break;
		case 1:
			printf("* Ruta: %s\n",param);
			break;
		case 2:
			printf("* Interfaz: %s\n",param);
			break;
		case 3:
			printf("* Metodo: %s\n",param);
			break;
		case 8:
			snprintf(byte,sizeof(byte),"%02hhX",self->msg[pos+4]);
			len = (int)strtol(byte, NULL, 16);
			if(len==0)
				break;
			printf("* Parametros:\n");
		    process_msg_args(self);
			break;
		}
		pos+=8+len+padding+1;
	}
	return 0;
}

static int send_message(dbusmessage_t* self, socket_t* skt) {
	int i, capacidad;
	capacidad = vector_obtener_cantidad(self->header)+vector_obtener_cantidad(self->body);
	char msg [capacidad];

	for(i=0; i<vector_obtener_cantidad(self->header); i++){
		vector_obtener(self->header,i,&msg[i]);
	}
	for(int j=0; j<vector_obtener_cantidad(self->body); j++){
		vector_obtener(self->body,j,&msg[i]);
		i++;
	}
	FILE *f;
	f = fopen("out.txt", "w");
	if (f == NULL) {
		printf("Error!\n");
		exit(1);
	}
    for(int i=0;(i<vector_obtener_cantidad(self->header)+vector_obtener_cantidad(self->body));i++){
    	fprintf(f,"%c", msg[i]);
    }
    fclose(f);
	return socket_send_message(skt, msg, vector_obtener_cantidad(self->header) + vector_obtener_cantidad(self->body));
}

int get_protocol_int(char* protocol,int start, int finish){
	char string[UINT32+1] = "";
	char c[3] = "";

	for (int i=start; i<finish; i++) {
		snprintf(c,sizeof(c),"%02hhX",protocol[i]);
		strcat(string,c);
	}
	return uint32_to_int(string);
}

static int get_msg_int(dbusmessage_t* self,int start, int finish){
	char string[UINT32+1] = "";
	char c[3] = "";

	for (int i=start; i<finish; i++) {
		snprintf(c,sizeof(c),"%02hhX",self->msg[i]);
		strcat(string,c);
	}
	return uint32_to_int(string);
}

int get_padding(int pos){
	int padding = 0;

	while (pos%8!=0) {
		padding++;
		pos++;
	}
	return padding;
}

static int get_cant_split(char* param){
	char temp[strlen(param)];
	strcpy(temp,param);
	char* ptr = NULL;
	int count = 0;

	ptr = strtok(temp,",");
	while (ptr!=NULL) {
		count++;
		ptr = strtok(NULL,",");
	}
	return count;
}

static int uint32_to_int(char* hex){
    int res = 0;
    char val[1];

    for (int i=0; i<UINT32; i++) {
        strncpy(val,&hex[i],1);
        switch (i) {
        case 0 :
         res+=(int)strtol(val, NULL, 16)*16;
         break;
        case 1 :
         res+=(int)strtol(val, NULL, 16);
         break;
        case 2 :
         res+=(int)strtol(val, NULL, 16)*16*16*16;
         break;
        case 3 :
         res+=(int)strtol(val, NULL, 16)*16*16;
         break;
        case 4 :
         res+=(int)strtol(val, NULL, 16)*16*16*16*16*16;
         break;
        case 5 :
         res+=(int)strtol(val, NULL, 16)*16*16*16*16;
         break;
        case 6 :
         res+=(int)strtol(val, NULL, 16)*16*16*16*16*16*16*16;
         break;
        case 7 :
         res+=(int)strtol(val, NULL, 16)*16*16*16*16*16*16;
         break;
        }
    }
    return res;
}

static void int_to_uint32(long int num, char* res) {
    long int val;
    char hexa[UINT32+1] = "";
    char temp[UINT32+1] = "";
	char aux[3];
	int i=0, hex;

	val = num;
	while(val!=0) {
		hex=val%16;
		if(hex<10) {
			hex=hex+48;
		} else {
			hex=hex+55;
		}
		temp[i]=hex;
		val=val/16;
		i++;
	}
	while(strlen(temp)<8){
	    temp[i]=48;
	    i++;
	}
	for (int j=0; j<UINT32; j+=2) {
	    snprintf(aux,sizeof(aux),"%c%c",temp[j+1],temp[j]);
	    strcat(hexa,aux);
	}
	strcpy(res,hexa);
}

static void update_protocol_len(dbusmessage_t* self) {
   self->lHeader = vector_obtener_cantidad(self->header);
   self->lBody = vector_obtener_cantidad(self->body);
}

static void set_protocol_process_destination(dbusmessage_t* self, char* destination) {
   set_protocol_param_const(self->header,6,1,'s');
   set_protocol_uint32(self->header,strlen(destination));
   set_protocol_param(self->header,destination);
   set_protocol_padding(self->header);
   update_protocol_len(self);
}

static void set_protocol_process_path(dbusmessage_t* self, char* path) {
   set_protocol_param_const(self->header,1,1,'o');
   set_protocol_uint32(self->header,strlen(path));
   set_protocol_param(self->header,path);
   set_protocol_padding(self->header);
   update_protocol_len(self);
}

static void set_protocol_process_interface(dbusmessage_t* self, char* interface) {
   set_protocol_param_const(self->header,2,1,'s');
   set_protocol_uint32(self->header,strlen(interface));
   set_protocol_param(self->header,interface);
   set_protocol_padding(self->header);
   update_protocol_len(self);
}

static void set_protocol_process_method(dbusmessage_t* self, char* method) {
   set_protocol_param_const(self->header,3,1,'s');
   set_protocol_uint32(self->header,strlen(method));
   set_protocol_param(self->header,method);
   set_protocol_padding(self->header);
   update_protocol_len(self);
}

static void set_protocol_process_args(dbusmessage_t* self,char* args) {
	char temp[strlen(args)], *arg;

	strcpy(temp,args);

	arg = strtok(temp, ",");

	set_protocol_param_const(self->header,8,1,'g');
	while (arg!=NULL) {
		//LONGITUD DEL ARGUMENTO EN 32BITS
		set_protocol_uint32(self->body,strlen(arg));
		//LETRAS DEL ARGUMENTO
		for (int i=0; i<strlen(arg); i++) vector_agregar(self->body,arg[i]);
		vector_agregar(self->body,0);//CORRESPONDE AL \0
		arg = strtok(NULL,",");
	}
	set_protocol_args(self,args);
	update_protocol_len(self);
	set_protocol_padding(self->header);
}

static void set_protocol_process_input(dbusmessage_t* self, char* input) {
	char destination[50], path[50], interface[50], method[50];
	char *param = NULL, temp[strlen(input)];
	int i = 0;

	strcpy(temp, input);
	for (int j = 0; j < strlen(temp); j++) if (temp[j]==')') temp[j] = '\0';

	param = strtok(temp,"(");
	while (param!=NULL) {
		switch (i) {
			case 0 :
				sscanf(param,"%s %s %s %s",destination, path, interface, method);
				set_protocol_process_destination(self,destination);
				set_protocol_process_path(self,path);
				set_protocol_process_interface(self,interface);
				set_protocol_process_method(self,method);
				break;
			case 1 :
				if (strlen(param) > 0)set_protocol_process_args(self,param);
		}
		i++;
		param = strtok(NULL,"(");
	}
}

static void set_protocol_param_const(vector_t* self,int id, int cnt, char type) {
   vector_agregar(self,id);//id param
   vector_agregar(self,cnt);//un string
   vector_agregar(self,type);//tipo de dato
   vector_agregar(self,0);//\0
}

static void set_protocol_header_constants(vector_t* self){
	vector_agregar(self,'l');//1er byte 'l'
	vector_agregar(self,1); //2do byte 1
	vector_agregar(self,0); //3er byte 0
	vector_agregar(self,1); //4to byte 1
}

static void set_protocol_param(vector_t* self,char* param) {
	for (int i=0; i<strlen(param); i++) {
	   vector_agregar(self,param[i]);
	}
	vector_agregar(self,0); //Agrego el byte de \0
}

static void set_protocol_padding(vector_t* self) {
	while ((vector_obtener_cantidad(self))%8!=0) {
		vector_agregar(self,0);
	}
}

static void set_protocol_args(dbusmessage_t* self, char* param) {
	int cantArgs = get_cant_split(param);

	vector_agregar(self->header,cantArgs);//CANTIDAD DE ARGUMENTOS
	for (int i=0; i<cantArgs; i++) {
		vector_agregar(self->header,'s');//TIPO DE DATO
	}
	vector_agregar(self->header,0);//AGREGO \0
}

static void set_protocol_init_uint32(vector_t* self){
	for (int i=0; i<UINT32/2; i++) {
		vector_agregar(self,0);
	}
}

static void set_protocol_uint32(vector_t* self, int num) {
	char num32[UINT32];
	char aux[3];

	int_to_uint32(num,num32);
	for (int i=0;i<UINT32;i+=2) {
		snprintf(aux,sizeof(aux),"%c%c",num32[i],num32[i+1]);
		vector_agregar(self,(int)strtol(aux, NULL, 16));
	}
}

static void set_protocol_uint32_custom(vector_t* self, int num, int s, int f) {
	char num32[UINT32];
	char aux[3];
	int j=0;

	int_to_uint32(num,num32);
	for (int i=s; i<f; i++) {
		snprintf(aux,sizeof(aux),"%c%c",num32[j],num32[j+1]);
		vector_guardar(self,i,(int)strtol(aux, NULL, 16));
		j+=2;
	}
}
