#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lista.h"
#include "hash.h"
#define CAPACIDAD_INICIAL_HASH 50
#define CANTIDAD_INICIAL_HASH 0
#define ACHICAR false
#define AGRANDAR true

typedef struct hash_campo{
	char* clave;
	void* valor;
} hash_campo_t;

struct hash{
	lista_t** listas;
	size_t capacidad;
	size_t cantidad;
	hash_destruir_dato_t destruir_dato;
};

struct hash_iter{
	size_t pos;
	lista_iter_t* iter_lista;
	const hash_t* hash;

};

size_t hashing(const char *key, size_t largo){ //jenkins_one_at_a_time_hash
    size_t pos_h, i;
    for(pos_h = i = 0; i < largo; ++i){
        pos_h += key[i];
        pos_h += (pos_h << 10);
        pos_h ^= (pos_h >> 6);
    }
    pos_h += (pos_h << 3);
    pos_h ^= (pos_h >> 11);
    pos_h += (pos_h << 15);
    return (pos_h);
}

hash_campo_t* buscar_campo_con_clave(lista_t* lista, const char*clave){
	lista_iter_t* iter = lista_iter_crear(lista);

	while(!lista_iter_al_final(iter)){ //RECORREMOS LA LISTA CORRESPONDIENTE HASTA ENCONTRAR EL CAMPO
		hash_campo_t* actual = lista_iter_ver_actual(iter);
		if (!(strcmp(actual->clave,clave))){
			lista_iter_destruir(iter);
			return actual;
		}
		lista_iter_avanzar(iter);
	}

	lista_iter_destruir(iter);
	return NULL;
}

bool hash_insertar(hash_t* hash , hash_campo_t* a_guardar, lista_t** listas, size_t capacidad){
	size_t pos = hashing(a_guardar->clave,strlen(a_guardar->clave))% capacidad;
 	if(!listas[pos]){ //SI NO TENGO UNA LISTA EN LA POSICION CORRESPONDIENTE DEL HASH
 		lista_t*lista = lista_crear();
 		if (!lista){
 			free(a_guardar->clave);
 			free(a_guardar);
			return false;
 		}
 		lista_insertar_ultimo(lista,a_guardar);
 		listas[pos] = lista;
 		hash->cantidad++;

 	}else if(lista_esta_vacia(listas[pos])){ //SI LA LISTA ESTÁ VACÍA
 		lista_insertar_ultimo(listas[pos],a_guardar);
 		hash->cantidad++;

 	}else{ //SI LA LISTA CORRESPONDIENTE NO ESTÁ VACÍA
 		hash_campo_t* campo = buscar_campo_con_clave(listas[pos],a_guardar->clave);
 		if (campo){
 			if (hash->destruir_dato) hash->destruir_dato(campo->valor);
 			campo->valor = a_guardar->valor;
 			free(a_guardar->clave);
 			free(a_guardar);
 		}else{
 			lista_insertar_ultimo(listas[pos],a_guardar);
 			hash->cantidad++;
 		}
 	}
 	return true;
}


/*
 * Redimensiona el hash, duplica su valor o lo parte a la mitad, dependiendo
 * de la variable accion. Debe recibir un hash válido.
 */
bool redimensionar2(hash_t* hash, bool accion){
	lista_t** nuevo = NULL;
	size_t nueva_capacidad = 0;

	if (accion) nuevo = malloc(sizeof(lista_t*)*2*hash->capacidad);
	else nuevo = malloc(sizeof(lista_t*)*hash->capacidad/2);
	if (!nuevo) return false;
	if (accion) nueva_capacidad = hash->capacidad *2;
	else nueva_capacidad = hash->capacidad/2;

	hash->cantidad=0;
	for (int i = 0;i < nueva_capacidad;i++){ nuevo[i] = NULL; } //RELLENO DE NULL
	for (int i = 0;i < hash->capacidad;i++){ // REHASHEO LOS ELEMENTOS
		if (hash->listas[i] != NULL){
			while(!lista_esta_vacia(hash->listas[i])){
				hash_campo_t* campo = lista_borrar_primero(hash->listas[i]);
				bool insertar = hash_insertar(hash,campo,nuevo,nueva_capacidad);
				if (!insertar){
					hash->cantidad = hash->cantidad;
					return false;
				}
			}
			lista_destruir(hash->listas[i],NULL);
		}
	}
	free(hash->listas);
	hash->listas = nuevo;
	hash->capacidad = nueva_capacidad;
	return true;
}

/*
 * Dada una clave y lista válida, busca el campo de la misma y lo devuelve.
 */



/*
 * Busca la siguiente lista no vacia para el iterador
 * Pre: El iterador fue creado
 * Post: Se cambiaron los atributos del iterador
 */
void buscar_no_vacia( hash_iter_t *iter){
	for(size_t i=iter->pos+1;i<iter->hash->capacidad;i++){ //BUSCO LA SIGUIENTE LISTA NO VACIA
		if(iter->hash->listas[i] && !lista_esta_vacia(iter->hash->listas[i])){
			lista_iter_t* nuevo_iter=lista_iter_crear(iter->hash->listas[i]);
			lista_iter_destruir(iter->iter_lista); //DESTRUYO EL VIEJO ITERADOR
			iter->iter_lista=nuevo_iter;
			iter->pos=i;
			return;
			}
		}
		iter->pos=iter->hash->capacidad;
		return;
}

/*
 * Crea el hash
 */
hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc(sizeof(hash_t));
	if (!hash){
		return NULL;
	}
	lista_t** listas = malloc(CAPACIDAD_INICIAL_HASH*sizeof(lista_t*));
	if(!listas){
		free(hash);
		return NULL;
	}
	hash->capacidad=CAPACIDAD_INICIAL_HASH;
	hash->cantidad=CANTIDAD_INICIAL_HASH;
	hash->listas=listas;
	hash->destruir_dato=destruir_dato;
	for (int i = 0;i < CAPACIDAD_INICIAL_HASH;i++){	hash->listas[i] = NULL; } //HAY QUE INICIALIZAR TODOS LOS VALORES CON NULL PARA QUE NO EXPLOTE SI QUEREMOS DESTRUIR UN HASH CON POSICIONES NO INICIALIZADAS
	return hash;
}

/*
 * Guarda un elemento en el hash, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura hash fue inicializada
 * Post: Se almacenó el par (clave, dato)
 */
 bool hash_guardar(hash_t *hash, const char *clave, void *dato){
 	if ((hash->cantidad/hash->capacidad)>=2){
 		bool redimension=redimensionar2(hash,AGRANDAR);
 		if(!redimension){return false;}
 	}
 	hash_campo_t* a_guardar = malloc(sizeof(hash_campo_t));
 	if (!a_guardar) return false;
 	char* copia_clave = malloc(sizeof(char)*(1+strlen(clave)));
	if(!copia_clave){
		free(a_guardar);
		return false;
	}
	strcpy(copia_clave,clave);
 	a_guardar->clave = copia_clave;
 	a_guardar->valor = dato;
 	bool insertar=hash_insertar(hash,a_guardar,hash->listas,hash->capacidad);
 	if (!insertar){
 		free(copia_clave);
 		free(a_guardar);
 		return false;
 	}
 	return true;
 }

/*
 * Borra un elemento del hash y devuelve el dato asociado.  Devuelve
 * NULL si el dato no estaba.
 * Pre: La estructura hash fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvió,
 * en el caso de que estuviera guardado.
 */
void *hash_borrar(hash_t *hash, const char *clave){
	if (!hash || hash->cantidad == 0){
		return NULL;
	}
	if (hash->cantidad<=hash->capacidad/4){
		redimensionar2(hash,ACHICAR);
	}
	size_t pos_h = (hashing(clave,strlen(clave)))%(hash->capacidad);
	if ((hash->listas)[pos_h] == NULL || lista_esta_vacia((hash->listas)[pos_h])) return NULL;
	lista_iter_t* iter = lista_iter_crear((hash->listas)[pos_h]);
	void* dato=NULL;

	while (!lista_iter_al_final(iter)){ //ITERAMOS EN LA LISTA CORRESPONDIENTE
		hash_campo_t* campo_actual = lista_iter_ver_actual(iter);
		char* clave_actual = campo_actual->clave;
		if (strcmp(clave_actual,clave) == 0){
			hash_campo_t* campo_dato = lista_iter_borrar(iter);
			free(clave_actual);
			dato = campo_dato->valor;
			free(campo_dato);
			break; //BORRAMOS EL DATO
		}
		lista_iter_avanzar(iter);
	}
	lista_iter_destruir(iter);
	hash->cantidad--;

	return dato;
}

/*
 * Obtiene el valor de un elemento del hash, si la clave no se encuentra
 * devuelve NULL.
 * Pre: La estructura hash fue inicializada
 */
void *hash_obtener(const hash_t *hash, const char *clave){
	if (!clave || !hash || hash->cantidad == 0){
		return NULL;
	}
	size_t pos_h = (hashing(clave,strlen(clave)))%(hash->capacidad);
	if (!hash->listas[pos_h] || lista_esta_vacia(hash->listas[pos_h])){
		return NULL;
	}
	hash_campo_t* campo = buscar_campo_con_clave(hash->listas[pos_h],clave);
	if (!campo) return NULL;
	return (campo->valor);
}

/* Determina si clave pertenece o no al hash.
 * Pre: La estructura hash fue inicializada
 */
 bool hash_pertenece(const hash_t *hash, const char *clave){
	if (!hash || hash->cantidad == 0){
		return false;
	}
 	size_t pos_h = hashing(clave,strlen(clave))%hash->capacidad;
 	if (!hash->listas[pos_h] || lista_esta_vacia((hash->listas)[pos_h])){
		return false;
	}
	hash_campo_t* buscado = buscar_campo_con_clave(hash->listas[pos_h],clave);
	return (buscado != NULL);
 }

/* Devuelve la cantidad de elementos del hash.
 * Pre: La estructura hash fue inicializada
 */
size_t hash_cantidad(const hash_t *hash){
	return hash->cantidad;
}

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * Pre: La estructura hash fue inicializada
 * Post: La estructura hash fue destruida
 */
void hash_destruir(hash_t *hash){
	if (!hash) return;
	for (int i = 0;i < hash->capacidad; i++){
		if (hash->listas[i] == NULL) {continue;}
		while(!lista_esta_vacia(hash->listas[i])){
			hash_campo_t* campo = lista_borrar_primero(hash->listas[i]);
			free(campo->clave);
			if(hash->destruir_dato){
			hash->destruir_dato(campo->valor);
			}
			free(campo);
		}
		lista_destruir(hash->listas[i],NULL);
	}
	free(hash->listas);
	free(hash);
}

/* Iterador del hash */

// Crea iterador
hash_iter_t *hash_iter_crear( const hash_t *hash){
	if (!hash) return NULL;
	hash_iter_t* iter = malloc(sizeof(hash_iter_t));
	if (!iter) return NULL;
	for (int i=0;i<hash->capacidad;i++){ //BUSCO LA PRIMER LISTA NO VACIA
		if (hash->listas[i] && !lista_esta_vacia(hash->listas[i])){
			iter->pos=i;
			lista_iter_t* iterador=lista_iter_crear(hash->listas[i]);
			iter->iter_lista=iterador;
			iter->hash=hash;
			return iter;
		}
	}
	iter->pos = hash->capacidad;
	iter->hash = hash;
	iter->iter_lista = NULL;
	return iter;
}

// Avanza iterador
bool hash_iter_avanzar(hash_iter_t *iter){
	if (hash_iter_al_final(iter)){
		return false;
	}
	if (!lista_iter_al_final(iter->iter_lista)){
		lista_iter_avanzar(iter->iter_lista);
		if (lista_iter_al_final(iter->iter_lista)){
			buscar_no_vacia(iter);
		}
	}
	return true;
}



// Devuelve clave actual, esa clave no se puede modificar ni liberar.
const char *hash_iter_ver_actual( const hash_iter_t *iter){
	if(iter->hash->cantidad == 0 || hash_iter_al_final(iter)) return NULL;
	hash_campo_t* campo = lista_iter_ver_actual(iter->iter_lista);
	return campo->clave;
}

// Comprueba si terminó la iteración
bool hash_iter_al_final( const hash_iter_t *iter){
	if(iter->hash->cantidad == 0 || iter->pos == iter->hash->capacidad) return true;
	return false;
}

// Destruye iterador
void hash_iter_destruir(hash_iter_t* iter){
	lista_iter_destruir(iter->iter_lista);
	free(iter);
}

// char** hash_iesimo_elemento(hash_t* hash, size_t i){
// 	if (hash == NULL || i > hash_cantidad(hash)) return NULL;
//
// }
