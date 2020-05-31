#ifndef COMMON_DBUSMESSAGE_H_
#define COMMON_DBUSMESSAGE_H_

#include "common_dynamicvector.h"
#include "common_socket.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/* El mensaje está planteado como un mensaje de strings. */

typedef struct {
	size_t id;
	char* msg;
	vector_t* header;
	vector_t* body;
	size_t lMsg;
	size_t lHeader;
	size_t lBody;
} dbusmessage_t;

/* ******************************************************************
 *                    PRIMITIVAS DEL MENSAJE
 * *****************************************************************/

// Pre: Crea un mensaje.
// Post: devuelve un mensaje nuevo.
void dbusmessage_create(dbusmessage_t* self);

// Pre: el mensaje fue creado.
// Post: se eliminaron todos los componentes del mensaje.
void dbusmessage_destroy(dbusmessage_t* self);

// Pre: el mensaje fue creado.
// Post: setea el id.
void dbusmessage_set_id(dbusmessage_t* self, int id);

// Pre: el mensaje fue creado.
// Post: devuelve el id.
int dbusmessage_get_id(dbusmessage_t* self);

// Pre: el mensaje fue creado.
// Post: setea el header.
void dbusmessage_set_header(dbusmessage_t* self, vector_t* header);

// Pre: el mensaje fue creado.
// Post: devuelve el header.
vector_t* dbusmessage_get_header(dbusmessage_t* self);

// Pre: el mensaje fue creado.
// Post: setea el body
void dbusmessage_set_body(dbusmessage_t* self, vector_t* body);

// Pre: el mensaje fue creado.
// Post: devuelve el body
vector_t* dbusmessage_get_body(dbusmessage_t* self);

// Convierte la entrada a formato protocolo
// Pre: el mensaje fue creado.
// Post: se asigna al mensaje el protocolo y se retorna
int dbusmessage_client_send(dbusmessage_t* self, socket_t* skt, char* input);

// Devuelve la longitud del mensage header
// Pre: el mensaje fue creado.
// Post: Retorna la longitud del header
size_t dbusmessage_client_get_len_header(dbusmessage_t* self);

// Devuelve la longitud del body
// Pre: el mensaje fue creado.
// Post: Retorna la longitud del body
size_t dbusmessage_client_get_len_body(dbusmessage_t* self);

// Recibe el mensaje del cliente
// Pre: el mensaje fue enviado.
// Post: se asigna el mensaje al dbus
int dbusmessage_server_recv(dbusmessage_t* self, socket_t* skt);

/* ******************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/
//Devuelve el valor entero del protocolo que se encuentra
//entre start y finish(server)
int get_protocol_int(char* protocol,int start, int finish);
//Devuelve la cantidad de padding existente en el header
//a partir de la posición pos(server)
int get_padding(int pos);

#endif /* COMMON_DBUSMESSAGE_H_ */
