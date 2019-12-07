#include <stdbool.h>
#include <stddef.h>

typedef struct lista lista_t;

typedef struct lista_iter lista_iter_t;

typedef struct nodo nodo_t;

// Crea un nodo.
// Post: devuelve el nodo creado, con el dato asignado y sin un próximo.
nodo_t* nodo_crear(void* dato);

//Crea un iterador que se actuará sobre la lista que entre por parametro.
//Pre: debe recibir una lista no vacía.
//Post: devuelve un iterador cuyo actual apuntará al primer valor de la lista,
//e inicialmente anterior apuntará al que viene antes de actual.
lista_iter_t *lista_iter_crear(lista_t *lista);

//Avanza el actual y el anterior del iterador, una posición hacia delante.
//Pre: debe recibir un iterador válido.
//Post: se devuelve true cuando se logre avanzar al iterador; en caso que el
//iterador sea NULL, esté vacía la lista, o esté al final el actual, se
//devolverá NULL.
bool lista_iter_avanzar(lista_iter_t *iter);

//Devuelve el valor del nodo actual.
//Pre: debe recibir un iterador, de largo no nulo, que no sea NULL y cuyo actual
//no se encuentre en NULL.
void *lista_iter_ver_actual(const lista_iter_t *iter);

//Devuelve true si está el actual en el final de la lista.
//Pre: recibe un iterador diferente de NULL.
//Post: devuelve true si está el actual en el final de la lista y si el anterior
//es el último valor de la lista sobre el cual existe el iterador.
bool lista_iter_al_final(const lista_iter_t *iter);

//Destruye el iterador. El mismo no debe ser nulo.
void lista_iter_destruir(lista_iter_t *iter);

//Inserta un valor en la posición del actual, es decir, entre el anterior y el
//actual.
//Pre: debe recibir un dato e iterador válidos.
//Post: inserta, en la posición correcta, el valor ingresado por parámetro
//dentro de un nodo nuevo; devuelve false en caso de que salga mal.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

//Borra el nodo en el cual actual está parado.
//Pre: debe ingresar un iterador distinto de NULL.
//Post: borra el nodo en el cual actual está parado, devuelve su dato y el nodo
//actual pasa a ser el siguiente del mismo.
void *lista_iter_borrar(lista_iter_t *iter);

//Destruye la lista y sus elementos dentro de ella.
//En caso de querer destruir estructuras complejas dentro de la misma, deberá
//recibir por parametro una función que se haga cargo de los mismos.
//Pre: obtiene una lista válida y una función de destrucción en caso de ser
//necesario.
//Post: destruye la lista y todos los elementos dentro de la misma.
void lista_destruir(lista_t *lista, void destruir_dato(void *));

//Es el iterador interno de la lista. Le aplica una función a todos los valores
//dentro de la lista.
//Pre: obtiene una lista y función válidas.
//Post: en caso de ser NULL la lista y/o la función, no ocurrirá nada, en caso
//contrario, se le aplicará la función a cada nodo de la lista y no se devolverá
//nada.
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);

// Crea una lista.
// Post: devuelve una nueva lista vacía, o NULL en caso de que ocurra un
// error al pedir memoria.
lista_t *lista_crear(void);

// Devuelve verdadero si la lista no tiene elementos, false en caso contrario.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

// Agrega un nuevo elemento a la lista en la primera posicion.
// Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista en la primera posicion; se
// actualiza el primero.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Agrega un nuevo elemento a la lista en la última posicion.
// Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista en la última posicion; se
// actualiza el último.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Borra el primer nodo de la lista y devuelve el dato; en caso de estar
// vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se eliminó el primer elemento de la lista; se actualiza el primero
void *lista_borrar_primero(lista_t *lista);

// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void *lista_ver_primero(const lista_t *lista);

// Obtiene el valor del último elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del último, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el último elemento de la lista, cuando no está vacía.
void *lista_ver_ultimo(const lista_t* lista);

//Devuelve el largo de la lista.
//Pre: la lista fue creada.
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void *));
