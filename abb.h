#include <stdbool.h>
#include <stddef.h>


// Definiciones de structs
typedef struct abb abb_t;
typedef struct n_abb n_abb_t;
typedef struct abb_iter abb_iter_t;


// Definicion de funcion de comparacion
typedef int (*abb_comparar_clave_t) (const char *, const char *);

// tipo de función para destruir dato
typedef void (*abb_destruir_dato_t) (void *);

//Crea el Arbol binario de busqueda
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

//Guarda la clave en la rama del arbol correspondiente. En caso
//de encontrarse en el arbol se reemplaza el dato.
//Pre: El abb fue creado.
//Post: Se guardo la clave en el abb
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

// Borra la clave correspondiente (si esta en la estructura) y devuelve el dato o NULL
//Pre: El abb fue creado
// Post: Se devolvio el dato correspondiente a la clave o NULL.
void *abb_borrar(abb_t *arbol, const char *clave);

// Busca la clave recibida por parametro y devuelve su dato en caso
// que este en la estructura. Null de lo contrario.
// Pre: El abb fue creado.
// Post; Se devolcio el dato correspondiente a la clave o NULL
void *abb_obtener(const abb_t *arbol, const char *clave);

//Devuelve True si la clave esta en la estructura, false
// en caso contrario
// Pre: El abb fue creado
bool abb_pertenece(const abb_t *arbol, const char *clave);

// Devuelve la cantidad de elementos almacenados en la estructura.
// Pre: El abb fue creado
// Post: Se devolvio la cantidad de elementos.
size_t abb_cantidad(abb_t *arbol);

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * Pre: La estructura hash fue inicializada
 * Post: La estructura hash fue destruida
 */
void abb_destruir(abb_t *arbol);


/* Iterador del Abb */

//Crea el iterador
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

// Avanza el iterador
bool abb_iter_in_avanzar(abb_iter_t *iter);

//Devuelve la clave actual
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

//Devuelve true si el iterador se encuentra al final. False en caso contrario
bool abb_iter_in_al_final(const abb_iter_t *iter);

//Destruye el iterador
void abb_iter_in_destruir(abb_iter_t* iter);

/* Iterador interno del Abb */

abb_iter_t *abb_iter_tablero(const abb_t *arbol,char* target);

void abb_in_order(abb_t *arbol, bool visitar(const char * /*clave*/, void */*dato*/, void */*extra*/), void *extra);

char *abb_iter_in_ver_dato_actual(const abb_iter_t *iter);

char** abb_obtener_rangos(abb_t* abb, size_t k, char* desde, char* hasta);
