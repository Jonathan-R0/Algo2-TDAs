#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "abb.h"
#include "pila.h"
#include "cola.h"
#define CANTIDAD_INICIAL_ABB 0

struct abb{
	n_abb_t* raiz;
	abb_destruir_dato_t destruir_dato;
	abb_comparar_clave_t comparar_clave;
	size_t cantidad;
};

struct n_abb{
	n_abb_t* izquierda;
	n_abb_t* derecha;
	void* dato;
	char* clave;
};

struct abb_iter{
	pila_t* elementos;
	};

struct buscar_nodo{
	n_abb_t* padre;
	n_abb_t* hijo;
};

typedef struct buscar_nodo buscar_nodo_t;

void abb_apilar_preorder(n_abb_t* arbol,pila_t* pila){
	if(!arbol)return;
	pila_apilar(pila,arbol);
	abb_apilar_preorder(arbol->izquierda,pila);
	abb_apilar_preorder(arbol->derecha,pila);

}

void abb_encolar_inorder(n_abb_t*arbol,cola_t*cola){
  if(!arbol) return;
  abb_encolar_inorder(arbol->izquierda,cola);
  cola_encolar(cola,arbol);
  abb_encolar_inorder(arbol->derecha,cola);
}

buscar_nodo_t* buscar_clave(n_abb_t* arbol, const char* clave,abb_comparar_clave_t comparar_clave,n_abb_t* padre){
  buscar_nodo_t*buscado;
  if(!arbol || comparar_clave(arbol->clave,clave)==0){
  	buscado=malloc(sizeof(buscar_nodo_t));
  	if(!buscado){return NULL;}
  	buscado->padre=padre;
  	buscado->hijo=arbol;

  }
  else if(comparar_clave(arbol->clave,clave)<0){
	return buscar_clave(arbol->derecha,clave,comparar_clave,arbol);
 }
  else {
	return buscar_clave(arbol->izquierda,clave,comparar_clave,arbol);
  }
  return buscado;
}

n_abb_t* buscar_reemplazante(n_abb_t*arbol){
	if(!arbol->izquierda){
		return arbol;
	}
	return buscar_reemplazante(arbol->izquierda);


}


//Crea el Arbol binario de busqueda
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
	abb_t* abb=malloc(sizeof(abb_t));
	if(!abb){return NULL;}
	abb->destruir_dato=destruir_dato;
	abb->comparar_clave=cmp;
	abb->raiz=NULL;
	abb->cantidad=CANTIDAD_INICIAL_ABB;
	return abb;
}

//Guarda la clave en la rama del arbol correspondiente. En caso
//de encontrarse en el arbol se reemplaza el dato.
//Pre: El abb fue creado.
//Post: Se guardo la clave en el abb
bool abb_guardar(abb_t *arbol, const char *clave, void *dato){
 bool valor=true;
 char* copia_clave = malloc(sizeof(char)*(1+strlen(clave)));
 if(!copia_clave){
	return false;
 }
 strcpy(copia_clave,clave);
 if(!arbol->raiz){ //SI NO HAY RAIZ PONGO LA CLAVE COMO RAIZ
   n_abb_t* raiz=malloc(sizeof(n_abb_t));
   if(!raiz){
	 free(copia_clave);
	 return false;
    }
	raiz->izquierda=NULL;
	raiz->derecha=NULL;
	raiz->clave=copia_clave;
	raiz->dato=dato;
	arbol->raiz=raiz;
	arbol->cantidad++;
	return valor;
}
 else{
 	buscar_nodo_t*buscado=buscar_clave(arbol->raiz,clave,arbol->comparar_clave,arbol->raiz);
 	if(!buscado){
 		free(copia_clave);
 		return false;}
 	if(!buscado->hijo){
   	n_abb_t* a_insertar=malloc(sizeof(n_abb_t));
   	if(!a_insertar){
   		free(copia_clave);
   		free(buscado);
   		return false;}
	a_insertar->clave=copia_clave;
	a_insertar->izquierda=NULL;
	a_insertar->derecha=NULL;
	a_insertar->dato=dato;
	if(arbol->comparar_clave(clave,buscado->padre->clave)>0){
		buscado->padre->derecha=a_insertar;
	}
	else{
		buscado->padre->izquierda=a_insertar;
	}
	arbol->cantidad++;
	free(buscado);
	}
   else{
   	  if(arbol->destruir_dato){
		arbol->destruir_dato(buscado->hijo->dato);
	 }
	 buscado->hijo->dato=dato;
	 free(copia_clave);
	 free(buscado);

	}
  }
  return valor;
}


// Borra la clave correspondiente (si esta en la estructura) y devuelve el dato o NULL
//Pre: El abb fue creado
// Post: Se devolvio el dato correspondiente a la clave o NULL.
void *abb_borrar(abb_t *arbol, const char *clave){
  void* a_devolver;
  buscar_nodo_t* buscado= buscar_clave(arbol->raiz,clave,arbol->comparar_clave,arbol->raiz);
  if(!buscado || !buscado->hijo){
  	free(buscado);
  	return NULL;}
  bool hijo_derecho=false;
  if(arbol->comparar_clave(buscado->hijo->clave,buscado->padre->clave)>0){ //ME FIJO SI ES EL HIJO DERECHO O EL IZQUIERDO DEL PADRE
  	hijo_derecho=true;
  }

  if(!buscado->hijo->derecha && !buscado->hijo->izquierda){// SI ES UNA HOJA LO ELIMINO Y ACTUALIZO AL PADRE
  	if(buscado->hijo==arbol->raiz){
  		arbol->raiz=NULL;
  	}
	if(hijo_derecho){

  	  buscado->padre->derecha=NULL;
  	}
  	else{
  	  buscado->padre->izquierda=NULL;
  	}
  	free(buscado->hijo->clave);
  	a_devolver=buscado->hijo->dato;
  	free(buscado->hijo);
  }
  else if((!buscado->hijo->derecha && buscado->hijo->izquierda) ||(buscado->hijo->derecha && !buscado->hijo->izquierda)){ // SI TIENE UN HIJO
  	n_abb_t*nieto; // HIJO DEL BUSCADO
  	if(buscado->hijo->derecha){
		nieto=buscado->hijo->derecha;
	}
  	else{
		nieto=buscado->hijo->izquierda;
	}
  	if(buscado->hijo==arbol->raiz){
  		arbol->raiz=nieto;
  	}
  	else if(hijo_derecho){   // DIGO QUE SU PADRE APUNTA A SU HIJO
  		buscado->padre->derecha=nieto;
  	}
  	else{
  		buscado->padre->izquierda=nieto;
  	}
  	free(buscado->hijo->clave);
  	a_devolver=buscado->hijo->dato;
  	free(buscado->hijo);
	}
  else if (buscado->hijo->derecha && buscado->hijo->izquierda){ //SI TIENE 2 HIJOS BUSCO EL REEMPLAZANTE, CAMBIO LOS DATOS POR LO DEL REEMPLANZATE BORRO AL REEMPLAZANTE Y ACTUALIZO LOS PADRES
  	n_abb_t* reemplazante=buscar_reemplazante(buscado->hijo->derecha);
  	n_abb_t*hijo_reemplazante=reemplazante->derecha;
  	buscar_nodo_t* buscar_reemplazante=buscar_clave(arbol->raiz,reemplazante->clave,arbol->comparar_clave,arbol->raiz);
  	n_abb_t* padre_reemplazante=buscar_reemplazante->padre;
    if(reemplazante!=buscado->hijo->derecha){
      padre_reemplazante->izquierda=hijo_reemplazante;
    }
  	else{
      buscado->hijo->derecha=hijo_reemplazante;
    }
  	char* reemplazante_clave=reemplazante->clave;
  	void* reemplazante_dato=reemplazante->dato;
    a_devolver=buscado->hijo->dato;
    free(buscado->hijo->clave);
  	buscado->hijo->clave=reemplazante_clave;
  	buscado->hijo->dato=reemplazante_dato;
    free(reemplazante);
    free(buscar_reemplazante);
  }

  free(buscado);
  arbol->cantidad--;
  return a_devolver;
}

// Busca la clave recibida por parametro y devuelve su dato en caso
// que este en la estructura. Null de lo contrario.
// Pre: El abb fue creado.
// Post; Se devolcio el dato correspondiente a la clave o NULL
void *abb_obtener(const abb_t *arbol, const char *clave){
buscar_nodo_t* buscado=buscar_clave(arbol->raiz,clave,arbol->comparar_clave,arbol->raiz);
if(!buscado->hijo){
	free(buscado);
	return NULL;}
void*dato= buscado->hijo->dato;
free(buscado);
return dato;

}
//Devuelve True si la clave esta en la estructura, false
// en caso contrario
// Pre: El abb fue creado
bool abb_pertenece(const abb_t *arbol, const char *clave){
 buscar_nodo_t* buscado=buscar_clave(arbol->raiz,clave,arbol->comparar_clave,arbol->raiz);
 if(!buscado->hijo){
 	free(buscado);
 	return false;}
 free(buscado);
 return true;
}

// Devuelve la cantidad de elementos almacenados en la estructura.
// Pre: El abb fue creado
// Post: Se devolvio la cantidad de elementos.
size_t abb_cantidad(abb_t *arbol){
	return arbol->cantidad;
}

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * Pre: La estructura hash fue inicializada
 * Post: La estructura hash fue destruida
 */
void abb_destruir(abb_t *arbol){
   pila_t* pila=pila_crear();
   if(!pila){return;}
   abb_apilar_preorder(arbol->raiz,pila);
   while(!pila_esta_vacia(pila)){
   	n_abb_t* actual=pila_desapilar(pila);
   	free(actual->clave);
   	if(arbol->destruir_dato){
   		arbol->destruir_dato(actual->dato);
   	}
   	free(actual);
	}
	pila_destruir(pila);
	free(arbol);
  }



/* Iterador del Abb */

//Crea el iterador
abb_iter_t *abb_iter_in_crear(const abb_t *arbol){
	if(!arbol){return NULL;}
	abb_iter_t* iterador=malloc(sizeof(abb_iter_t));
	if(!iterador){return NULL;}
	pila_t* elementos=pila_crear();
	if(!elementos){
		free(iterador);
		return NULL;}
	iterador->elementos=elementos;
	n_abb_t* actual=arbol->raiz;
	while(actual){ //APILO LA RAIZ Y TODOS LOS HIJOS IZQUIERDOS
		pila_apilar(iterador->elementos,actual);
		actual=actual->izquierda;
	}
	return iterador;
}

// Avanza el iterador
bool abb_iter_in_avanzar(abb_iter_t *iter){
  if(abb_iter_in_al_final(iter)){return false;}
  n_abb_t* desapilado=pila_desapilar(iter->elementos);
  n_abb_t* hijo_derecho=desapilado->derecha;
  if(hijo_derecho){
    pila_apilar(iter->elementos,hijo_derecho);
	n_abb_t* actual=hijo_derecho->izquierda;
	while(actual){
	pila_apilar(iter->elementos,actual);
	actual=actual->izquierda;
	}
}
return true;
}


//Devuelve la clave actual
const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
 n_abb_t* tope= pila_ver_tope(iter->elementos);
 if (tope){
   return tope->clave;}
 return NULL;
}

//Devuelve true si el iterador se encuentra al final. False en caso contrario
bool abb_iter_in_al_final(const abb_iter_t *iter){
  return pila_esta_vacia(iter->elementos);
}
//Destruye el iterador
void abb_iter_in_destruir(abb_iter_t* iter){
	pila_destruir(iter->elementos);
	free(iter);
}

void _abb_in_order(n_abb_t*arbol,bool visitar(const char*clave,void*dato,void*extra),void*extra){
  if(!arbol) return;
  cola_t*cola=cola_crear();
  if(!cola) return;
  abb_encolar_inorder(arbol,cola);
  while(!cola_esta_vacia(cola)){
    n_abb_t*actual=cola_desencolar(cola);
    bool valor=visitar(actual->clave,actual->dato,extra);
    if(!valor) break;
  }
  cola_destruir(cola,NULL);
}

void abb_in_order(abb_t *arbol, bool visitar(const char *clave, void *dato, void *extra), void *extra){
    _abb_in_order(arbol->raiz,visitar,extra);
}

//Crea un iterador con los elementos del árbol que estén en el rango
abb_iter_t *abb_iter_tablero(const abb_t *arbol,char* target){
	if(!arbol) return NULL;
	abb_iter_t* iterador = malloc(sizeof(abb_iter_t));
	if(!iterador) return NULL;
	pila_t* elementos = pila_crear();
	if(!elementos){
		free(iterador);
		return NULL;
	}
	iterador->elementos = elementos;
	n_abb_t* actual = arbol->raiz;
	while(actual){ //APILO LA RAIZ Y TODOS LOS HIJOS IZQUIERDOS
		pila_apilar(iterador->elementos,actual);
		actual = actual->izquierda;
		if (actual != NULL && arbol->comparar_clave(actual->clave,target) <= 0){
			actual = actual->derecha;
		}
	}
	return iterador;
}

char *abb_iter_in_ver_dato_actual(const abb_iter_t *iter){
	n_abb_t* tope = pila_ver_tope(iter->elementos);
	if (tope) return tope->dato;
	return NULL;
}
