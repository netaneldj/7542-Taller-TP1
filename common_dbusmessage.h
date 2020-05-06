#ifndef COMMON_DBUSMESSAGE_H_
#define COMMON_DBUSMESSAGE_H_

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "common_dynamicvector.h"

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/* El mensaje está planteado como un mensaje de strings. */

typedef struct {
	size_t id;
	char* msg;
	char* destination;
	char* path;
	char* interface;
	char* method;
	char** args;
	vector_t* header;
	vector_t* body;
	size_t lMsg;
	size_t lHeader;
	size_t lBody;
	size_t lArgs;
} dbusmessage_t;

/* ******************************************************************
 *                    PRIMITIVAS DEL MENSAJE
 * *****************************************************************/

// Pre: Crea un mensaje.
// Post: devuelve un mensaje nuevo.
dbusmessage_t* dbusmessage_create();

// Pre: el mensaje fue creado.
// Post: se eliminaron todos los componentes del mensaje.
int dbusmessage_destroy(dbusmessage_t* self);

// Pre: el mensaje fue creado.
// Post: setea el id.
int dbusmessage_set_id(dbusmessage_t* self, int id);

// Pre: el mensaje fue creado.
// Post: devuelve el id.
int dbusmessage_get_id(dbusmessage_t* self);

// Convierte la entrada a formato protocolo
// Pre: el mensaje fue creado.
// Post: se asigna al mensaje el protocolo y se retorna
char* dbusmessage_client_get_protocol(dbusmessage_t* self, char* input);

// Devuelve la longitud del mensage protcolo
// Pre: el mensaje fue creado.
// Post: Retorna la longitud del protocolo
size_t dbusmessage_client_get_len_protocol(dbusmessage_t* self);

// Devuelve la longitud del mensage header
// Pre: el mensaje fue creado.
// Post: Retorna la longitud del header
size_t dbusmessage_client_get_len_header(dbusmessage_t* self);

// Devuelve la longitud del body
// Pre: el mensaje fue creado.
// Post: Retorna la longitud del body
size_t dbusmessage_client_get_len_body(dbusmessage_t* self);

// Convierte el protocolo a formato entrada
// Pre: el mensaje fue creado.
// Post: se asigna la entrada al mensaje y se retorna
int dbusmessage_server_set_message(dbusmessage_t* self, char* msg, size_t len);

// Retorna el destino del mensaje
// Pre: el mensaje fue asignado.
// Post: se retorna el destino del mensaje
char* dbusmessage_server_get_destination(dbusmessage_t* self);

// Retorna la ruta del mensaje
// Pre: el mensaje fue asignado.
// Post: retorna la ruta del mensaje
char* dbusmessage_server_get_path(dbusmessage_t* self);

// Retorna la interface del mensaje
// Pre: el mensaje fue asignado.
// Post: retorna la interface del mensaje
char* dbusmessage_server_get_interface(dbusmessage_t* self);

// Retorna el metodo del mensaje
// Pre: el mensaje fue asignado.
// Post: retorna el metodo del mensaje
char* dbusmessage_server_get_method(dbusmessage_t* self);

// Retorna los argumentos del mensaje
// Pre: el mensaje fue asignado.
// Post: retorna los argumentos del mensaje
char** dbusmessage_server_get_args(dbusmessage_t* self);

// Retorna la cantidad de argumentos del mensaje
// Pre: el mensaje fue asignado.
// Post: retorna la cantiadad de argumentos del mensaje
size_t dbusmessage_server_get_cant_args(dbusmessage_t* self);

/* ******************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/
//Devuelve el valor entero del protocolo que se encuentra entre start y finish(server)
int get_protocol_int(char* protocol,int start, int finish);
//Devuelve la cantidad de padding existente en el header a partir de la posición pos(server)
int get_padding(int pos);

#endif /* COMMON_DBUSMESSAGE_H_ */
