#ifndef DYNAMICVECTOR_H
#define DYNAMICVECTOR_H

#include <stdlib.h>
#include <stdbool.h>

/*******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 ******************************************************************/

typedef struct vector {
    size_t capacidad;
    size_t cantidad;
    char* datos;
} vector_t;


/*******************************************************************
 *                    PRIMITIVAS DEL VECTOR
 ******************************************************************/

// Crea un vector de tamaño tam
// Post: vector es una vector vacío de tamaño tam
void vector_crear(vector_t* self, size_t capacidad);

// Destruye el vector
// Pre: el vector fue creado
// Post: se eliminaron todos los elementos del vector
void vector_destruir(vector_t* vector);

// Almacena en valor el dato guardado en la posición pos del vector
// Pre: el vector fue creado
// Post: se almacenó en valor el dato en la posición pos. Devuelve false si la
// posición es inválida (fuera del rango del vector, que va de 0 a tamaño-1)
bool vector_obtener(vector_t* vector, size_t pos, char* valor);

// Almacena el valor en la posición pos
// Pre: el vector fue creado
// Post: se almacenó el valor en la posición pos. Devuelve false si la posición
// es inválida (fuera del rango del vector, que va de 0 a tamaño-1) y true si
// se guardó el valor con éxito.
bool vector_guardar(vector_t* vector, size_t pos, char valor);

// Almacena el valor en la última posición contigua del vector
// Pre: el vector fue creado
// Post: se almacenó el valor en la última posición. Devuelve true si
// se guardó el valor con éxito.
bool vector_agregar(vector_t* vector, char valor);

// Devuelve la capacidad del vector
// Pre: el vector fue creado
size_t vector_obtener_capacidad(vector_t* vector);

// Devuelve la cantidad de elementos almacenados en el vector
// Pre: el vector fue creado
size_t vector_obtener_cantidad(vector_t* vector);


#endif // DYNAMICVECTOR_H
