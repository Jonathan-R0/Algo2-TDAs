#include <stdlib.h>
#include <stdbool.h>
#include "cola.h"

typedef struct nodo{
	void* valor;
	struct nodo* prox;
}nodo_t;

struct cola{
	nodo_t* prim;
	nodo_t* ult;
};

// Crea un nodo.
// Post: devuelve el nodo creado, con el dato asignado y sin un próximo.
nodo_t* nodo_crear1(void* dato){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if (nodo == NULL) return NULL;
	nodo->valor = dato;
	nodo->prox = NULL;
	return nodo;
}

// Crea una cola.
// Post: devuelve una nueva cola vacía.
cola_t* cola_crear(void){
	cola_t* cola = malloc(sizeof(cola_t));
	if (cola == NULL) return NULL;
	cola->prim = NULL;
	cola->ult = NULL;
	return cola;
}

// Destruye la cola. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la cola llama a destruir_dato.
// Pre: la cola fue creada. destruir_dato es una función capaz de destruir
// los datos de la cola, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la cola y la cola.
void cola_destruir(cola_t *cola, void destruir_dato(void*)){
	while (!cola_esta_vacia(cola)){
		if (destruir_dato != NULL){
			(destruir_dato)(cola_desencolar(cola));
		} else {
			cola_desencolar(cola);
		}
	}
	free(cola);
}


// Devuelve verdadero si la cola no tiene elementos encolados, false en caso contrario.
// Pre: la cola fue creada.
bool cola_esta_vacia(const cola_t *cola){
	return (cola->prim == NULL && cola->ult == NULL);
}

// Agrega un nuevo elemento a la cola. Devuelve falso en caso de error.
// Pre: la cola fue creada.
// Post: se agregó un nuevo elemento a la cola, valor se encuentra al final
// de la cola.
bool cola_encolar(cola_t *cola, void* valor){
	nodo_t* nodo = nodo_crear1(valor);
	if (nodo == NULL) return false;
	if (cola_esta_vacia(cola)){
		cola->prim = nodo;
	} else {
		cola->ult->prox = nodo;
	}
	cola->ult = nodo;
	return true;
}

// Obtiene el valor del primer elemento de la cola. Si la cola tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la cola fue creada.
// Post: se devolvió el primer elemento de la cola, cuando no está vacía.
void* cola_ver_primero(const cola_t *cola){
	if (cola->prim == NULL) return NULL;
	return (cola->prim->valor);
}

// Saca el primer elemento de la cola. Si la cola tiene elementos, se quita el
// primero de la cola, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la cola fue creada.
// Post: se devolvió el valor del primer elemento anterior, la cola
// contiene un elemento menos, si la cola no estaba vacía.
void* cola_desencolar(cola_t *cola){
	if (cola->ult == NULL || cola->prim == NULL) return NULL;
	void* data = cola->prim->valor;
	nodo_t* a_eliminar = cola->prim;
	if (cola->prim->prox == NULL && cola->ult->prox == NULL){
		cola->prim = NULL;
		cola->ult = NULL;
	} else {
		cola->prim = cola->prim->prox;
	}
	free(a_eliminar);
	return data;
}
