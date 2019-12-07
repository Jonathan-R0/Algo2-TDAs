#include "lista.h"
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

struct nodo{
	void* valor;
	struct nodo* prox;
};

struct lista{
	struct nodo* prim;
	struct nodo* ult;
	size_t largo;
};

struct lista_iter{
	lista_t* lista;
	nodo_t* anterior;
	nodo_t* actual;
};

// Crea un nodo.
// Post: devuelve el nodo creado, con el dato asignado y sin un próximo.
nodo_t* nodo_crear2(void* dato){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if (nodo == NULL){
		return NULL;
	}
	nodo->valor = dato;
	nodo->prox = NULL;
	return nodo;
}

//Crea un iterador que se actuará sobre la lista que entre por parametro.
//Pre: debe recibir una lista no vacía.
//Post: devuelve un iterador cuyo actual apuntará al primer valor de la lista,
//e inicialmente anterior apuntará al que viene antes de actual.
lista_iter_t *lista_iter_crear(lista_t *lista){
	lista_iter_t* iter = malloc(sizeof(lista_iter_t));
	if (iter == NULL){
		return NULL;
	}
	iter->lista = lista;
	iter->actual = iter->lista->prim;
	iter->anterior = NULL;
	return iter;
}

//Avanza el actual y el anterior del iterador, una posición hacia delante.
//Pre: debe recibir un iterador válido.
//Post: se devuelve true cuando se logre avanzar al iterador; en caso que el
//iterador sea NULL, esté vacía la lista, o esté al final el actual, se
//devolverá NULL.
bool lista_iter_avanzar(lista_iter_t *iter){
	if (iter == NULL || lista_iter_al_final(iter)){
		return false;
	}
	iter->anterior = iter->actual;
	iter->actual = iter->actual->prox;
	return true;
}

//Devuelve el valor del nodo actual.
//Pre: debe recibir un iterador, de largo no nulo, que no sea NULL y cuyo actual
//no se encuentre en NULL.
void *lista_iter_ver_actual(const lista_iter_t *iter){
	if (iter == NULL || lista_iter_al_final(iter)){
		return NULL;
	}
	return(iter->actual->valor);
}

//Devuelve true si está el actual en el final de la lista.
//Pre: recibe un iterador diferente de NULL.
//Post: devuelve true si está el actual en el final de la lista y si el anterior
//es el último valor de la lista sobre el cual existe el iterador.
bool lista_iter_al_final(const lista_iter_t *iter){
	if (iter == NULL){
		return false;
	}
	return(iter->actual == NULL || iter->anterior == iter->lista->ult);
}

//Destruye el iterador. El mismo no debe ser nulo.
void lista_iter_destruir(lista_iter_t *iter){
	free(iter);
}

//Inserta un valor en la posición del actual, es decir, entre el anterior y el
//actual.
//Pre: debe recibir un dato e iterador válidos.
//Post: inserta, en la posición correcta, el valor ingresado por parámetro
//dentro de un nodo nuevo; devuelve false en caso de que salga mal.
bool lista_iter_insertar(lista_iter_t *iter, void *dato){
	nodo_t* nodo = nodo_crear2(dato);
	if (iter == NULL || nodo == NULL){
		free(nodo);
		return false;
	}
	iter->lista->largo++;
	if (iter->anterior == NULL){
		iter->lista->prim = nodo;
		if (iter->actual == NULL){
			iter->lista->ult = nodo;
		} else {
			nodo->prox = iter->actual;
		}
	} else {
		iter->anterior->prox = nodo;
		if (lista_iter_al_final(iter)){
			iter->lista->ult = nodo;
    	} else {
			nodo->prox = iter->actual;
		}
	}
	iter->actual = nodo;
	return true;
}

//Borra el nodo en el cual actual está parado.
//Pre: debe ingresar un iterador distinto de NULL.
//Post: borra el nodo en el cual actual está parado, devuelve su dato y el nodo
//actual pasa a ser el siguiente del mismo.
void *lista_iter_borrar(lista_iter_t *iter){
	if (iter == NULL || iter->actual == NULL){
		return NULL;
	}
	nodo_t* a_eliminar = iter->actual;
	void* data = a_eliminar->valor;
	iter->lista->largo--;
	iter->actual = iter->actual->prox;
	if (iter->anterior == NULL){
		iter->lista->prim = iter->actual;
	} else {
		iter->anterior->prox = iter->actual;
		if (iter->actual == NULL && iter->anterior != NULL){
			iter->lista->ult = iter->anterior;
		}
	}
	free(a_eliminar);
	return data;
}

//Crea una lista y la devuelve.
lista_t *lista_crear(void){
    lista_t* lista = malloc(sizeof(lista_t));
    if (lista == NULL){
        return NULL;
    }
    lista->prim = NULL;
    lista->ult = NULL;
    lista->largo = 0;
    return lista;
}

//Devuelve true en caso de estar vacía, false en caso contrario.
//Pre: no debe recibir NULL, pero si ocurre, devuelve false.
bool lista_esta_vacia(const lista_t *lista){
	if (lista == NULL) return false;
    return(lista->largo == 0);
}

//Inserta el dato en el primer nodo de la lista.
//Pre: recibe un dato y lista válidos y no nulos.
//Post: devuelve true cuando el nuevo nodo con el dato fue insertado en la
//primera posición; false en caso contrario.
bool lista_insertar_primero(lista_t *lista, void* dato){
    nodo_t* nodo = nodo_crear2(dato);
    if (nodo == NULL){
        free(nodo);
        return false;
    }
    if (lista_esta_vacia(lista)){
        lista->ult = nodo;
    }
	lista->largo++;
    nodo_t* aux = lista->prim;
    lista->prim = nodo;
	lista->prim->prox = aux;
    return true;
}

//Inserta el dato en el último nodo de la lista.
//Pre: recibe un dato y lista válidos y no nulos.
//Post: devuelve true cuando el nuevo nodo con el dato fue insertado en la
//última posición; false en caso contrario.
bool lista_insertar_ultimo(lista_t *lista, void *dato){
    nodo_t* nodo = nodo_crear2(dato);
    if (nodo == NULL){
        return false;
    }
	if (lista_esta_vacia(lista)){
		lista->prim = nodo;
    } else {
    	lista->ult->prox = nodo;
	}
	lista->ult = nodo;
	lista->largo++;
    return true;
}

//Borra el primer nodo de la lista, y devuelve su valor.
//Pre: recibe una lista válida.
//Post: borra el primer nodo y devuelve su dato o  devuelve NULL en caso de
//estar vacía.
void* lista_borrar_primero(lista_t *lista){
    if (lista_esta_vacia(lista)){
        return NULL;
    }
    nodo_t* aux = lista->prim;
    lista->prim = aux->prox;
	void* dato = aux->valor;
    free(aux);
	lista->largo--;
    return(dato);
}

//Devuelve el valor del primer nodo de la lista.
//Pre: recibe una lista válida.
//Post: devuelve el dato del primer nodo o NULL en caso de estar vacía.
void *lista_ver_primero(const lista_t *lista){
    if (lista_esta_vacia(lista)){
        return NULL;
    }
    return(lista->prim->valor);
}

//Devuelve el valor del último nodo de la lista.
//Pre: recibe una lista válida.
//Post: devuelve el dato del último nodo o NULL en caso de estar vacía.
void *lista_ver_ultimo(const lista_t* lista){
    if (lista_esta_vacia(lista)){
        return NULL;
    }
    return(lista->ult->valor);
}

//Devuelve el largo de la lista.
//Pre: recibe una lista válida.
//Post: devuelve el largo de la misma.
size_t lista_largo(const lista_t *lista){
	return(lista->largo);
}

//Destruye la lista y sus elementos dentro de ella.
//En caso de querer destruir estructuras complejas dentro de la misma, deberá
//recibir por parametro una función que se haga cargo de los mismos.
//Pre: obtiene una lista válida y una función de destrucción en caso de ser
//necesario.
//Post: destruye la lista y todos los elementos dentro de la misma.
void lista_destruir(lista_t *lista, void destruir_dato(void *)){
    while (!lista_esta_vacia(lista)){
        if (destruir_dato != NULL){
            (destruir_dato)(lista_borrar_primero(lista));
        } else {
            lista_borrar_primero(lista);
        }
    }
    free(lista);
}

//Es el iterador interno de la lista. Le aplica una función a todos los valores
//dentro de la lista.
//Pre: obtiene una lista y función válidas.
//Post: en caso de ser NULL la lista y/o la función, no ocurrirá nada, en caso
//contrario, se le aplicará la función a cada nodo de la lista y no se devolverá
//nada.
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra){
	if (lista == NULL || visitar == NULL || lista_esta_vacia(lista)) return;
	nodo_t* actual = lista->prim;
	while(actual != NULL && visitar(actual->valor,extra)){
		actual = actual->prox;
	}
}
