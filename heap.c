#define _GNU_SOURCE
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "heap.h"
#define TAMANIO_INICIAL 15 //Particularmente este es nuestro tamanio
//inicial ya que corresponde con un hash de 4 niveles o filas
#define DISMINUIR false
#define EXPANDIR true

struct heap{
	void ** datos;
	size_t cant;
	size_t tam;
	cmp_func_t cmp;
};

void swap(void** a, void** b){
    void* c = *a;
    *a = *b;
    *b = c;
}

void upheap(heap_t* heap,size_t pos){
	if (!pos) return;
	size_t padre = (pos-1)/2;
	if (heap->cmp(heap->datos[padre], heap->datos[pos]) < 0){
        swap(&heap->datos[padre],&heap->datos[pos]);
		upheap(heap,padre);
    }
}

void downheap(void** datos, size_t cant, size_t pos, cmp_func_t cmp){
	if (cant == 1 || pos >= cant/2) return; //SI ES UNA HOJA O HAY 1 ELEMENTO EN EL HEAP NO HAGO NADA
	size_t padre = pos;
	size_t izq = 2 * pos + 1;
	size_t der = 2 * pos + 2;
	if (izq < cant &&cmp(datos[izq], datos[padre]) > 0) padre = izq;
	if (der < cant &&cmp(datos[der], datos[padre]) > 0) padre = der;
	if (padre != pos){
        swap(&datos[padre],&datos[pos]);
		downheap(datos,cant,padre,cmp);
    }
}

bool heap_esta_vacio(const heap_t *heap){
    if (!heap) return false;
    return (!heap->cant);
}

heap_t* heap_crear(cmp_func_t cmp){
    if (!cmp) return NULL;
    heap_t* heap = malloc(sizeof(heap_t));
    if (!heap) return NULL;
    heap->cmp = cmp;
    heap->tam = TAMANIO_INICIAL;
    heap->datos = malloc((heap->tam) * sizeof(void*));
    if (!heap->datos ){
        free(heap);
        return NULL;
    }
	heap->cant = 0;
    return heap;
}

void heap_destruir(heap_t *heap, void destruir_elemento(void *e)){
    if (!heap) return;
    if (destruir_elemento){
		 for (size_t pos = 0;pos < heap->cant;pos++){
			 destruir_elemento(heap->datos[pos]);
		 }
	}
    free(heap->datos);
    free(heap);
}

size_t heap_cantidad(const heap_t *heap){
    if (!heap || heap_esta_vacio(heap)) return 0;
    return (heap->cant);
}

void *heap_ver_tope(const heap_t *heap){
    if (!heap || heap_esta_vacio(heap)) return NULL;
    return (heap->datos[0]);
}

bool redimensionar(heap_t* heap, bool ajuste){
    if (heap == NULL) return false;
    if (ajuste == EXPANDIR){
        heap->tam = (heap->tam*2) + 1;
    } else {
        heap->tam = heap->tam/2;
    }
    void** data_aux = realloc(heap->datos, heap->tam*sizeof(void*));
    if (data_aux == NULL){
		if (ajuste == EXPANDIR) heap->tam = (heap->tam - 1)/2;
		else heap->tam = heap->tam*2;
        return false;
    }
	heap->datos = data_aux;
	if (ajuste == EXPANDIR){
		for (size_t i = heap->cant; i < heap->tam; i++){ heap->datos[i] = NULL; }
	}
    return true;
}

bool heap_encolar(heap_t *heap, void *elem){
    if (!heap|| !elem ) return false;
    if (heap->cant == heap->tam) redimensionar(heap,EXPANDIR);
    heap->datos[heap->cant] = elem;
	heap->cant++;
    upheap(heap,heap->cant-1);
    return true;
}


void *heap_desencolar(heap_t *heap){
    if (!heap || heap_esta_vacio(heap)) return NULL;
    void* a_devolver = heap->datos[0];
    heap->datos[0] = heap->datos[heap->cant-1];
    heap->cant--;
    downheap(heap->datos,heap->cant,0,heap->cmp);
    if (heap->cant*2 <= heap->tam && heap->cant >= TAMANIO_INICIAL) redimensionar(heap,DISMINUIR);
    return a_devolver;
}

heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp){ //Cambiar por heapify
    if (!arreglo || n < 1) return NULL;
    heap_t* heap = heap_crear(cmp);
    if(!heap) return NULL;
    if(n > TAMANIO_INICIAL) redimensionar(heap,EXPANDIR); //Redimensiono si tengo mas elementos que la capacidad
    for(int i = 0; i < n;i++){
   		heap->datos[i] = arreglo[i]; //Copio los elementos en el heap
    }
    for(int i = (int)n - 1;i > -1;i--){
    	downheap(heap->datos,n,i,cmp);  //Doy forma de Heap
    }
	heap->cant = n;
	return heap;
}

void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp){
    if (!elementos|| cant <= 1 || cmp == NULL) return;
	for(int i = (int)cant-1;i > -1;i--){
    	downheap(elementos,cant,i,cmp);  //Doy forma de Heap
    }
	size_t segmento = cant;
	for (size_t i = 0; i < cant;i++){
        swap(&elementos[0],&elementos[segmento-1]);
		downheap(elementos,segmento-1,0,cmp);  //downheap de la raiz para el nuevo segmento
		segmento--;
    }
	return;
}
