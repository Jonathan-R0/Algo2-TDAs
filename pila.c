#include "pila.h"
#include <stdlib.h>

/* Definición del struct pila proporcionado por la cátedra.*/

#define CAPACIDAD_INICAL 32
#define DISMINUIR false
#define EXPANDIR true

struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/*******************************************************************
 *                    PRIMITIVAS DE LA PILA                        *
 *******************************************************************/

 // Crea una pila.
 // Post: devuelve una nueva pila vacía.
pila_t* pila_crear(void){
    pila_t* pila = malloc(sizeof(pila_t));
    pila->capacidad = CAPACIDAD_INICAL;
    pila->cantidad = 0;
    if (pila == NULL){
        return NULL;
    }
    pila->datos = malloc((pila->capacidad) * sizeof(void*));
    if (pila->datos == NULL){
        free(pila->datos);
        return NULL;
    }
    return pila;
}

 // Destruye la pila.
 // Pre: la pila fue creada.
 // Post: se eliminaron todos los elementos de la pila.
void pila_destruir(pila_t *pila){
    if (pila){
        free(pila->datos);
    } //WIP
    free(pila);
}

 // Devuelve verdadero si la pila no tiene elementos apilados, false en caso contrario.
 // Pre: la pila fue creada.
bool pila_esta_vacia(const pila_t *pila){
    return (pila->cantidad == 0);
}

 // Obtiene el valor del tope de la pila. Si la pila tiene elementos,
 // se devuelve el valor del tope. Si está vacía devuelve NULL.
 // Pre: la pila fue creada.
 // Post: se devolvió el valor del tope de la pila, cuando la pila no está
 // vacía, NULL en caso contrario.
void* pila_ver_tope(const pila_t *pila){
    if (pila->cantidad == 0){
        return NULL;
    }
    return (pila->datos[(pila->cantidad)-1]);
}

 // Redimensiona la pila en caso de ser necesario. Devuelve false de el realloc
 // devuleva NULL. En caso contrario se sobrescriben los datos de la pila con la
 // redimensión efenctuada.
 // Pre: la pila fue creada, y en caso de que se la quiera agrandar el segundo
 // parámetro debe ser "EXPANDIR", "DISMINUIR" en caso contrario.
 // Post: en caso de que ocurra un error en el realloc, se mantienen los datos
 // y la capacidad intactos y se devuelve false; si no ocurre ningún error se
 // sobrescriben los datos y se devuelve true.
bool redimensionar1(pila_t* pila, bool ajuste){
    if (ajuste == EXPANDIR){
        pila->capacidad = pila->capacidad*2;
    } else {
        pila->capacidad = pila->capacidad/2;
    }
    void** data_aux = realloc(pila->datos, pila->capacidad*sizeof(void*));
    if (data_aux == NULL && ajuste == EXPANDIR){
        pila->capacidad = pila->capacidad/2;
        return false;
    }
    if (data_aux == NULL && ajuste == DISMINUIR){
        pila->capacidad = pila->capacidad*2;
        return false;
    }
    pila->datos = data_aux;
    return true;
}

 // Agrega un nuevo elemento a la pila. Devuelve falso en caso de error.
 // Pre: la pila fue creada.
 // Post: se agregó un nuevo elemento a la pila, valor es el nuevo tope (si
 // en algún momento el booleano que se devuleve se hace false, se devolverá
 // true cuando se consiga realizar un realloc sin errores)
bool pila_apilar(pila_t *pila, void* valor){
    if (pila->capacidad == pila->cantidad){
        bool respuesta = redimensionar1(pila,2);
        if (respuesta == false){
            return false;
        }
    }
    pila->datos[pila->cantidad] = valor;
    pila->cantidad += 1;
    return true;
}

 // Saca el elemento tope de la pila. Si la pila tiene elementos, se quita el
 // tope de la pila, y se devuelve ese valor. Si la pila está vacía, devuelve
 // NULL.
 // Pre: la pila fue creada.
 // Post: si la pila no estaba vacía, se devuelve el valor del tope anterior
 // y la pila contiene un elemento menos.
void* pila_desapilar(pila_t *pila){
    if (pila->cantidad == 0){
        return NULL;
    }
    if (pila->capacidad > 4*pila->cantidad && (pila->capacidad/2) > CAPACIDAD_INICAL){
        redimensionar1(pila,DISMINUIR);
    }
    void* ultimo_elemento = pila->datos[(pila->cantidad) - 1];
    pila->cantidad -= 1;
    return (ultimo_elemento);
}
