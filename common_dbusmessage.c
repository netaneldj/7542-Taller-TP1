#include "common_dbusmessage.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "common_dynamicvector.h"

#define UINT32 8
#define PARAMETERS 5
#define BUFFER_SIZE 32

/* ******************************************************************
 *                DEFINICION FUNCIONES AUXILIARES
 * *****************************************************************/
//Guarda espacio para un uint32 en el mensaje
static void set_protocol_init_uint32(vector_t* self);
//Escribe el numero en formato uint32
static void set_protocol_uint32(vector_t* self, int num);
//Escribe el numero en formato uint32 desde start hasta finish
static void set_protocol_uint32_custom(vector_t* self, int num, int start, int finish);
//Agrega los datos de la firma del mensaje
static void set_protocol_args(dbusmessage_t* self, char* param);
//Agrega las constantes del parametro al mensaje
static void set_protocol_padding(vector_t* self);
//Escribe el parametro en el mensaje(client)
static void set_protocol_param(vector_t* self,char* param);
//Establece las constantes de los parametrso del mensaje(client)
static void set_protocol_param_constants(vector_t* self,int paramId, int cant, char dataType);
//Agrega el encabezado del header(client)
static void set_protocol_header_constants(vector_t* self);
//Procesa los parametros del mensaje(client)
static void set_protocol_process_input(dbusmessage_t* self, char* input);
//Procesa los argumentos del metodo(client)
static void set_protocol_process_args(vector_t* self,char* param);
//Actualiza la longitud del header y del body del protocolo(client)
static void update_protocol_len(dbusmessage_t* self);
//Convierte de entero a uint32
static void int_to_uint32(long int num, char* res);
//Convierte de hexadecimal de 4 bytes a entero
static int uint32_to_int(char* uint32);
//Devuelve una nueva cadena reemplazando las ocurrencias de la palabra anterior con la nueva
static char* replace_word(char* string, char *oldWord, char *newWord);
//Devuelve la cantidad de elementos existentes en el split de la cadena param con el delimitador delim(client)
static int get_cant_split(char* param, char delim);
//Escribe el mensaje unificado a retornar con el header y el body(client)
static int write_message(dbusmessage_t* self);
//Devuelve el valor decimal del entero de 4 bytes almacenado en el mensaje entre las posiciones start y finish(server)
static int get_msg_int(dbusmessage_t* self,int start, int finish);
//Procesa los parametros del mensaje y los guarda en sus respectivas variables dentro del mensaje(server)
static int process_msg_parameters(dbusmessage_t* self);
//Procesa los argumentos del metodo y los guarda en la lista de argumentos del mensaje(server)
static int process_msg_args(dbusmessage_t* self);

/* ******************************************************************
 *                IMPLEMENTACION
 * *****************************************************************/
void dbusmessage_create(dbusmessage_t* self){
	self->id = 0;
	self->msg = NULL;
	self->destination = NULL;
	self->path = NULL;
	self->interface = NULL;
	self->method = NULL;
	self->args = NULL;
	vector_crear(&self->header,0);
	vector_crear(&self->body,0);
	self->lHeader = 0;
	self->lBody = 0;
	self->lMsg = 0;
	self->lArgs = 0;
}

void dbusmessage_destroy(dbusmessage_t* self){
	vector_destruir(&self->header);
	vector_destruir(&self->body);
	free(self->destination);
	free(self->path);
	free(self->interface);
	free(self->method);
	free(self->msg);
	if(self->lArgs>0){
		for(int i=0;i<self->lArgs;i++)
			free(self->args[i]);
	}
	free(self->args);
}

void dbusmessage_set_id(dbusmessage_t* self, int id){
	self->id = id;
}

int dbusmessage_get_id(dbusmessage_t* self){
	return self->id;
}

char* dbusmessage_client_get_protocol(dbusmessage_t* self, char* input){
	set_protocol_header_constants(&self->header);
	set_protocol_init_uint32(&self->header);
	set_protocol_uint32(&self->header,dbusmessage_get_id(self));
	set_protocol_init_uint32(&self->header);
	set_protocol_process_input(self,input);
	set_protocol_uint32_custom(&self->header,(int)dbusmessage_client_get_len_body(self),4,8);
	set_protocol_uint32_custom(&self->header,(int)dbusmessage_client_get_len_header(self),12,16);
	write_message(self);
	return self->msg;
}

size_t dbusmessage_client_get_len_protocol(dbusmessage_t* self){
	return vector_obtener_cantidad(&self->header)+vector_obtener_cantidad(&self->body);
}

//la suma de len header y len body no es la misma que len protocol porque ahi se cuenta el padding del ultimo parametro
size_t dbusmessage_client_get_len_header(dbusmessage_t* self){
	return self->lHeader;
}
size_t dbusmessage_client_get_len_body(dbusmessage_t* self){
	return self->lBody;
}

int dbusmessage_server_set_message(dbusmessage_t* self, char* msg, size_t len){
	self->msg = malloc(len*sizeof(char));
	if (len > 0 && self->msg == NULL) {
	        return 1;
	}
	self->msg = msg;
	self->lMsg = len;
	self->lBody = get_msg_int(self,4,8);
	self->id = get_msg_int(self,8, 12);
	self->lHeader = get_msg_int(self,12,16);
	process_msg_parameters(self);
	return 0;
}

char* dbusmessage_server_get_destination(dbusmessage_t* self){
	return self->destination;
}

char* dbusmessage_server_get_path(dbusmessage_t* self){
	return self->path;
}

char* dbusmessage_server_get_interface(dbusmessage_t* self){
	return self->interface;
}

char* dbusmessage_server_get_method(dbusmessage_t* self){
	return self->method;
}

char** dbusmessage_server_get_args(dbusmessage_t* self){
	return self->args;
}

size_t dbusmessage_server_get_cant_args(dbusmessage_t* self){
	return self->lArgs;
}

/* ******************************************************************
 *                IMPLEMENTACIONES AUXILIARES
 * *****************************************************************/

static int process_msg_args(dbusmessage_t* self){
	int pos = self->lMsg-self->lBody;
	int len;
	int i = 0;

	while(pos<self->lMsg){
		len = get_msg_int(self,pos,pos+4);
		self->args[i] = malloc((len+1)*sizeof(char));
	    if (len>0 && self->args == NULL) {
	        return -1;
	    }
		strncpy(self->args[i],self->msg+pos+4,len+1);
		i++;
		pos+=4+len+1;
	}
	return 0;
}

static int process_msg_parameters(dbusmessage_t* self){
	int pos = 16;
	int tipo, len, padding;
	char byte[3] = "";

	while (pos<self->lHeader) {
		sprintf(byte,"%02hhX",self->msg[pos]);
		tipo = (int)strtol(byte, NULL, 16);
		len = get_msg_int(self,pos+4,pos+8);
		padding = get_padding(pos+8+len+1);
		switch(tipo){
		case 6:
			self->destination = malloc((len+1)*sizeof(char));
		    if (self->destination == NULL) {
		        return -1;
		    }
			strncpy(self->destination,self->msg+pos+8,len+1);
			break;
		case 1:
			self->path = malloc((len+1)*sizeof(char));
		    if (self->path == NULL) {
		        return -1;
		    }
			strncpy(self->path,self->msg+pos+8,len+1);
			break;
		case 2:
			self->interface = malloc((len+1)*sizeof(char));
		    if (self->interface == NULL) {
		        return -1;
		    }
			strncpy(self->interface,self->msg+pos+8,len+1);
			break;
		case 3:
			self->method = malloc((len+1)*sizeof(char));
		    if (self->method == NULL) {
		        return -1;
		    }
			strncpy(self->method,self->msg+pos+8,len+1);
			break;
		case 8:
			sprintf(byte,"%02hhX",self->msg[pos+4]);
			len = (int)strtol(byte, NULL, 16);
			self->lArgs = len;
			if(len==0)
				break;
			self->args = (char**) malloc(len*sizeof(char*));
		    if (len>0 && self->args == NULL) {
		        return -1;
		    }
		    process_msg_args(self);
			break;
		}
		pos+=8+len+padding+1;
	}
	return 0;
}

int get_protocol_int(char* protocol,int start, int finish){
	char string[UINT32+1] = "";
	char c[3] = "";

	for (int i=start;i<finish;i++) {
		sprintf(c,"%02hhX",protocol[i]);
		strcat(string,c);
	}
	return uint32_to_int(string);
}

static int get_msg_int(dbusmessage_t* self,int start, int finish){
	char string[UINT32+1] = "";
	char c[3] = "";

	for (int i=start;i<finish;i++) {
		sprintf(c,"%02hhX",self->msg[i]);
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

static int write_message(dbusmessage_t* self) {
	int i, capacidad;

	capacidad = vector_obtener_cantidad(&self->header)+vector_obtener_cantidad(&self->body);
	self->msg = malloc(capacidad*sizeof(char));
    if (capacidad > 0 && self->msg == NULL) {
        return -1;
    }
	for(i=0;i<vector_obtener_cantidad(&self->header);i++){
		vector_obtener(&self->header,i,&self->msg[i]);
	}
	for(int j=0;j<vector_obtener_cantidad(&self->body);j++){
		vector_obtener(&self->body,j,&self->msg[i]);
		i++;
	}
	return 0;
}

static int get_cant_split(char* param, char delim){
	char* ptr = param;
	char format[] = "%31[^ ]%n";
	char field[BUFFER_SIZE];
	int n, count;

	count = 0;
	format[5] = delim;
	while (sscanf(ptr, format, field, &n) == 1) {
		count++;
		ptr += n;
		if ( *ptr != delim ) {
			break;
		}
		++ptr;
	}
	return count;
}

static char* replace_word(char* string, char *oldWord, char *newWord) {
	char *result = NULL;
    int i, count = 0;
    int newWlen = strlen(newWord);
    int oldWlen = strlen(oldWord);

    for (i = 0; string[i] != '\0'; i++) {
        if (strstr(&string[i], oldWord) == &string[i]) {
            count++;
            i += oldWlen - 1;
        }
    }

    result = (char *)malloc(i + count * (newWlen - oldWlen) + 1);
    i = 0;
    while (*string) {
        if (strstr(string, oldWord) == string) {
            strcpy(&result[i], newWord);
            i += newWlen;
            string += oldWlen;
        } else
            result[i++] = *string++;
    }

    result[i] = '\0';
    return result;
}

static int uint32_to_int(char* hex){
    int res = 0;
    char val[1];

    for (int i=0;i<UINT32;i++) {
        strncpy(val,&hex[i],1);
        switch(i) {
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
	    sprintf(aux,"%c%c",temp[j+1],temp[j]);
	    strcat(hexa,aux);
	}
	strcpy(res,hexa);
}

static void update_protocol_len(dbusmessage_t* self) {
   self->lHeader = vector_obtener_cantidad(&self->header);
   self->lBody = vector_obtener_cantidad(&self->body);
}

static void set_protocol_process_args(vector_t* self,char* param) {
	char arg[BUFFER_SIZE], lenArg[UINT32], aux[3];
	char format[] = "%31[^,]%n";
	char* ptr = param;
	int n;

	while (sscanf(ptr, format, arg, &n) == 1) {
	//LONGITUD DEL ARGUMENTO EN 32BITS
	int_to_uint32(strlen(arg),lenArg);
	for (int i=0;i<UINT32;i+=2) {
		sprintf(aux, "%c%c",lenArg[i],lenArg[i+1]);
		vector_agregar(self,(int)strtol(aux, NULL, 16));
	}
	//LETRAS DEL ARGUMENTO
	for(int i=0;i<strlen(arg);i++){
	   vector_agregar(self,arg[i]);
	}
	vector_agregar(self,0);//CORRESPONDE AL \0
	ptr += n;
	if ( *ptr != ',' ) {
		break;
	}
	++ptr;
	}
}

static void set_protocol_process_input(dbusmessage_t* self, char* input) {
	char *msg, *param;
	int params;

	msg = replace_word(input,"("," ");
	msg = replace_word(msg,")","");
	params = get_cant_split(msg,' ');
	param = strtok(msg," ");
	for (int i=0;i<params;i++) {
		switch(i) {
		   case 0 : //DESTINO
			   set_protocol_param_constants(&self->header,6,1,'s');
			   set_protocol_uint32(&self->header,strlen(param));
			   set_protocol_param(&self->header,param);
			   set_protocol_padding(&self->header);
			   update_protocol_len(self);
			   break;
		   case 1 : //RUTA
			   set_protocol_param_constants(&self->header,1,1,'o');
			   set_protocol_uint32(&self->header,strlen(param));
			   set_protocol_param(&self->header,param);
			   set_protocol_padding(&self->header);
			   update_protocol_len(self);
			   break;
		   case 2 : //INTERFAZ
			   set_protocol_param_constants(&self->header,2,1,'s');
			   set_protocol_uint32(&self->header,strlen(param));
			   set_protocol_param(&self->header,param);
			   set_protocol_padding(&self->header);
			   update_protocol_len(self);
			   break;
		   case 3 : //METODO
			   set_protocol_param_constants(&self->header,3,1,'s');
			   set_protocol_uint32(&self->header,strlen(param));
			   set_protocol_param(&self->header,param);
			   set_protocol_padding(&self->header);
			   update_protocol_len(self);
			   break;
		   case 4 : //ARGUMENTOS
			   set_protocol_param_constants(&self->header,8,1,'g');
			   set_protocol_process_args(&self->body,param);
			   set_protocol_args(self,param);
			   update_protocol_len(self);
			   set_protocol_padding(&self->header);
			   break;
		}
		param = strtok(NULL," ");
	}

}

static void set_protocol_param_constants(vector_t* self,int paramId, int cant, char dataType) {
   vector_agregar(self,paramId);//id param
   vector_agregar(self,cant);//un string
   vector_agregar(self,dataType);//tipo de dato
   vector_agregar(self,0);//\0
}

static void set_protocol_header_constants(vector_t* self){
	vector_agregar(self,'l');//1er byte 'l'
	vector_agregar(self,1); //2do byte 1
	vector_agregar(self,0); //3er byte 0
	vector_agregar(self,1); //4to byte 1
}

static void set_protocol_param(vector_t* self,char* param) {
	for (int i=0;i<strlen(param);i++) {
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
	int cantArgs = get_cant_split(param,',');

	vector_agregar(&self->header,cantArgs);//CANTIDAD DE ARGUMENTOS
	for (int i=0;i<cantArgs;i++) {
		vector_agregar(&self->header,'s');//TIPO DE DATO
	}
	vector_agregar(&self->header,0);//AGREGO \0
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
		sprintf(aux, "%c%c",num32[i],num32[i+1]);
		vector_agregar(self,(int)strtol(aux, NULL, 16));
	}
}

static void set_protocol_uint32_custom(vector_t* self, int num, int start, int finish) {
	char num32[UINT32];
	char aux[3];
	int j=0;

	int_to_uint32(num,num32);
	for (int i=start;i<finish;i++) {
		sprintf(aux, "%c%c",num32[j],num32[j+1]);
		vector_guardar(self,i,(int)strtol(aux, NULL, 16));
		j+=2;
	}
}
