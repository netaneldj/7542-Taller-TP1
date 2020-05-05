#include "DynamicVector.h"

void vector_destruir(vector_t* vector){
	free(vector->datos);
	free(vector);
}

bool vector_obtener(vector_t* vector, size_t pos, char* valor){
	if(pos<0 || pos>=vector->capacidad){
		return false;
	}
	*valor=vector->datos[pos];
	return true;
}

bool vector_guardar(vector_t* vector, size_t pos, char valor){
	if(pos<0){
		return false;
	}
	if(vector->cantidad>=vector->capacidad/2){
		if(!vector_redimensionar(vector,vector->capacidad*2+1)){
			return false;
		}
	}
	vector->datos[pos]=valor;
	return true;
}

bool vector_agregar(vector_t* vector, char valor){
	if(vector->cantidad>=vector->capacidad/2){
		if(!vector_redimensionar(vector,vector->capacidad*2+1)){
			return false;
		}
	}
	vector->datos[vector->cantidad]=valor;
	vector->cantidad++;
	return true;
}

size_t vector_obtener_capacidad(vector_t* vector){
	return vector->capacidad;
}

size_t vector_obtener_cantidad(vector_t* vector){
	return vector->cantidad;
}

vector_t* vector_crear(size_t capacidad) {
    vector_t* vector = malloc(sizeof(vector_t));

    if (vector == NULL) {
        return NULL;
    }
    vector->datos = malloc(capacidad * sizeof(char));

    if (capacidad > 0 && vector->datos == NULL) {
        free(vector);
        return NULL;
    }
    vector->capacidad = capacidad;
    vector->cantidad = 0;
    return vector;
}

bool vector_redimensionar(vector_t* vector, size_t nueva_cap) {
    char* datos_nuevo = realloc(vector->datos, nueva_cap * sizeof(char));

    // Cuando tam_nuevo es 0, es correcto si se devuelve NULL.
    // En toda otra situación significa que falló el realloc.
    if (nueva_cap > 0 && datos_nuevo == NULL) {
        return false;
    }

    vector->datos = datos_nuevo;
    vector->capacidad = nueva_cap;
    return true;
}
