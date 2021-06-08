#define _GNU_SOURCE
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "abb.h"
#include "cola.h"
#include "hash.h"
#include "heap.h"
#include "lista.h"
#include "pila.h"
#include "strutil.h"

#define TAM_MAX_INPUT 1000
#define N_ERROR -2
#define MODO_DE_APERTURA "r"
#define SEPARADOR_COMA ','
#define SEPARADOR_ESPACIO ' '
#define ERROR "Error en comando"
#define AGREGAR_ARCHIVO "agregar_archivo"
#define VER_TABLERO "ver_tablero"
#define INFO_VUELO "info_vuelo"
#define PRIORIDAD_VUELOS "prioridad_vuelos"
#define BORRAR "borrar"
#define ASCENDENTE "asc"
#define DESCENDENTE "desc"
#define SEPARADOR_T 'T'

#define ERROR_STATUS -1
#define OK_STATUS 0

/******************************
 *                             *
 *    Funciones adicionales    *
 *                             *
 ******************************/

/*
 *  Verifica que un char* válido y no nulo corresponda a un número natural.
 *  Devuelve true en caso de que esto se cumpla, false en caso contrario.
 */
bool is_digit(char* op, size_t len) {
  if (op == NULL || len == 0) return false;
  for (int j = 0; j < len; j++) {
    if (!isdigit(op[j])) return false;
  }
  return true;
}

/*
 * Recibe un arreglo no válido y devuelve su largo.
 */
size_t strv_cantidad(char** strv, size_t cont) {
  while (strv[cont] != NULL) {
    cont++;
  }
  return cont;
}

/*
 * Recibe un char* diferente del NULL de tipo AAAA-MM-DDYHH-MM-SS y genera un
 * char** donde, en el orden antes mencionado, devuelve una lista con los datos
 * correspondientes a la fecha correcta.
 */
char** obtener_fechas(const char* day, const char* time) {
  char** dia = split(day, '-');
  char** horario = split(time, ':');
  char** a_devolver = malloc(sizeof(char*) * 7);
  for (int i = 0; i < 3; i++) {
    a_devolver[i] = strdup(dia[i]);
    a_devolver[i + 3] = strdup(horario[i]);
  }
  a_devolver[6] = NULL;  // PARA PODER USAR FREE_STRV
  free_strv(dia);
  free_strv(horario);
  return a_devolver;
}

/*
 * Dada una fecha genera una clave de árbol, la cual posee la fecha pasada
 * y una prioridad nula. También verifica que sean válidas.
 */
char* generar_fecha(char* day) {
  char** fecha = split(day, SEPARADOR_T);
  if (fecha == NULL || strv_cantidad(fecha, 0) != 2) {
    free_strv(fecha);
    return NULL;
  }
  char** date = obtener_fechas(fecha[0], fecha[1]);
  for (int i = 0; i < 6; i++) {
    if (!is_digit(date[i], strlen(date[i]))) {
      free_strv(fecha);
      free_strv(date);
      return NULL;
    }
  }
  char* arreglo[] = {fecha[0], fecha[1], "0000", NULL};
  char* a_devolver = join(arreglo, SEPARADOR_T);
  free_strv(fecha);
  free_strv(date);
  return (a_devolver);
}

/*
 * Pre: recibe un vuelo válido.
 * Post: devuelve la clave generada para el correspondiente vuelo.
 */
char* generar_clave_hash(char* linea) {
  char** vuelo = split(linea, SEPARADOR_COMA);
  char* a_devolver = strdup(vuelo[0]);
  free_strv(vuelo);
  return a_devolver;
}

/*
 * Pre: recibe un vuelo válido.
 * Post: devuelve la clave generada para el correspondiente vuelo.
 */
char* generar_clave_abb(const char* linea) {
  char** vuelo = split(linea, SEPARADOR_COMA);
  char** pre_clave = malloc(sizeof(char*) * 3);
  pre_clave[0] = vuelo[6];
  pre_clave[1] = vuelo[0];
  pre_clave[2] = NULL;
  char* a_devolver = join(pre_clave, SEPARADOR_T);
  free_strv(vuelo);
  free(pre_clave);
  return (a_devolver);
}

/*
 * Función de comparación del abb. Compara dos vuelos que entran por parámetro.
 * Post: devuelve 1 si el primer vuelo es mayor al segundo, -1 en caso
 * contrario, 0 si son iguales y -2 en caso de error, es decir, si alguno es
 * NULL.
 */
int cmp_abb(const char* info1, const char* info2) {
  if (info1 == NULL || info2 == NULL) return N_ERROR;
  char** vuelo1 = split(info1, SEPARADOR_T);
  char** vuelo2 = split(info2, SEPARADOR_T);
  char** fechas1 = obtener_fechas(vuelo1[0], vuelo1[1]);
  char** fechas2 = obtener_fechas(vuelo2[0], vuelo2[1]);
  int valor = 0;
  for (int i = 5; i > -1; i--) {
    if (atoi(fechas1[i]) > atoi(fechas2[i]))
      valor = 1;
    else if (atoi(fechas1[i]) < atoi(fechas2[i]))
      valor = -1;
  }
  free_strv(fechas1);
  free_strv(fechas2);
  if (valor == 0) valor = strcmp(vuelo1[2], vuelo2[2]);
  free_strv(vuelo1);
  free_strv(vuelo2);
  return valor;
}

/*
 * Función de comparación del heap.
 * Pre: recibe la info correspondiente a dos vuelos y los compara según su
 * prioridad.
 * Post: devuelve 1 en caso de que la prioridad del primer vuelo sea la mayor,
 * -1 en caso contrario y 0 en caso de que sean iguales.
 */
int cmp_prioridad(const void* info1, const void* info2) {
  char** line1 = split((char*)info1, SEPARADOR_COMA);
  char** line2 = split((char*)info2, SEPARADOR_COMA);
  int prioridad1 = atoi(line1[5]);
  int prioridad2 = atoi(line2[5]);
  int rta = 0;
  if (prioridad1 > prioridad2) rta = -1;
  if (prioridad1 < prioridad2) rta = 1;
  if (rta == 0) rta = strcmp(line1[0], line2[0]);
  free_strv(line1);
  free_strv(line2);
  return rta;
}

/*
 * Compara dos fechas sin tener en cuenta su código de vuelo.
 */
int cmp_abb2(const char* info1, const char* info2) {
  if (info1 == NULL || info2 == NULL) return N_ERROR;
  char** vuelo1 = split(info1, SEPARADOR_T);
  char** vuelo2 = split(info2, SEPARADOR_T);
  char** fechas1 = obtener_fechas(vuelo1[0], vuelo1[1]);
  char** fechas2 = obtener_fechas(vuelo2[0], vuelo2[1]);
  int valor = 0;
  for (int i = 5; i > -1; i--) {
    if (atoi(fechas1[i]) > atoi(fechas2[i]))
      valor = 1;
    else if (atoi(fechas1[i]) < atoi(fechas2[i]))
      valor = -1;
  }
  free_strv(fechas1);
  free_strv(fechas2);
  free_strv(vuelo1);
  free_strv(vuelo2);
  return valor;
}

/*
 * Pre: recibe fechas válidas
 * Post: imprime la info del i-esimo vuelo si está en el rango de fechas
 */
void imprimir_tablero(char* desde, char* hasta, char** tablero_asc, size_t i) {
  char** info = split(tablero_asc[i], SEPARADOR_COMA);
  char* n_desde = generar_fecha(desde);
  char* n_hasta = generar_fecha(hasta);
  char* a_comparar = generar_clave_abb(tablero_asc[i]);
  if ((cmp_abb(a_comparar, n_desde) >= 0 &&
       cmp_abb(a_comparar, n_hasta) <= 0) ||
      (cmp_abb2(a_comparar, n_hasta) == 0))
    printf("%s - %s\n", info[6], info[0]);
  free(a_comparar);
  free(n_desde);
  free(n_hasta);
  free_strv(info);
}

/*
 * Dado un abb no válido, un nro cantidad y dos fechas delimitadoras
 * consigue una lista con los elementos que, inorden, están dentro
 * del rango dado en el abb.
 */
char** obtener_rangos(abb_t* abb, char* desde, char* hasta) {
  char* n_desde = generar_fecha(desde);
  abb_iter_t* iter = abb_iter_tablero(abb, n_desde);
  size_t i = 1;
  char** vuelos_en_rango = malloc(sizeof(char*) * (abb_cantidad(abb) + 2));
  vuelos_en_rango[0] = NULL;
  if (vuelos_en_rango == NULL) {
    abb_iter_in_destruir(iter);
    free(n_desde);
    return false;
  }
  char* n_hasta = generar_fecha(hasta);
  if (cmp_abb(abb_iter_in_ver_actual(iter), n_desde) >= 0 &&
      cmp_abb(abb_iter_in_ver_actual(iter), n_hasta) <= 0) {
    vuelos_en_rango[i++] = abb_iter_in_ver_dato_actual(iter);
  }
  while (abb_iter_in_avanzar(iter) &&
         cmp_abb(abb_iter_in_ver_actual(iter), n_hasta) <= 0) {
    if (cmp_abb(abb_iter_in_ver_actual(iter), n_desde) >= 0) {
      vuelos_en_rango[i++] = abb_iter_in_ver_dato_actual(iter);
    }
  }
  if (cmp_abb2(abb_iter_in_ver_actual(iter), n_hasta) == 0) {
    vuelos_en_rango[i++] = abb_iter_in_ver_dato_actual(iter);
  }
  free(n_desde);
  free(n_hasta);
  abb_iter_in_destruir(iter);
  vuelos_en_rango[i] = NULL;

  return vuelos_en_rango;
}

/*
 * Recibe dos fechas y confirma que la primera venga antes que la segunda
 * Pre: recibe dos fechas válidas.
 * Post: devuelve true si el 'desde' viene después que el 'hasta', false
 * en caso contrario.
 */
bool fechas_incorrectas(char* desde, char* hasta) {
  char* n_desde = generar_fecha(desde);
  char* n_hasta = generar_fecha(hasta);
  if (n_desde == NULL || n_hasta == NULL || cmp_abb(n_desde, n_hasta) > 0) {
    free(n_desde);
    free(n_hasta);
    return true;
  }
  free(n_desde);
  free(n_hasta);
  return false;
}

/******************************
 *                             *
 *         Primitivas          *
 *                             *
 ******************************/

/*
 * Lee el archivo que se le pase por parámetro y almacena los vuelos en el hash
 * y abb no nulos dados.
 * Pre: recibe un hash y abb válidos
 * Post: almacena en estos los vuelos, con la fecha como clave en el abb y la
 * prioridad de vuelo como clave en el hash (el valor asociado siempre es
 * la info del vuelo). Devuelve false en caso de que haya algún error, true
 * en caso de que la operación termine exitosamente.
 */
bool read_file(const char* file, hash_t* hash, abb_t* abb) {
  if (file == NULL) return false;
  char* linea = NULL;
  size_t capacidad = 0;
  size_t leidos = 0;

  FILE* archivo = fopen(file, MODO_DE_APERTURA);
  if (archivo == NULL) return false;
  cola_t* cola = cola_crear();

  while ((leidos = getline(&linea, &capacidad, archivo)) != -1) {
    if (leidos == 1) continue;

    if (linea == NULL || !cola_encolar(cola, strdup(linea))) {
      cola_destruir(cola, NULL);
      free(linea);
      fclose(archivo);
      return false;
    }
  }
  free(linea);
  fclose(archivo);

  while (!cola_esta_vacia(cola)) {
    char* vuelo1 = (char*)cola_desencolar(cola);

    char* clave_abb = generar_clave_abb(vuelo1);
    char* clave_hash = generar_clave_hash(vuelo1);
    if (clave_abb == NULL || clave_hash == NULL) {
      free(clave_abb);
      free(clave_hash);
      abb_destruir(abb);
      hash_destruir(hash);
      free(vuelo1);
      break;
    }

    char* vuelo2 = strdup(vuelo1);
    char* repetido = NULL;

    if ((repetido = hash_obtener(hash, clave_hash)) != NULL) {
      char* clave_a_borrar = generar_clave_abb(repetido);
      free(abb_borrar(abb, clave_a_borrar));
      free(clave_a_borrar);
      free(hash_borrar(hash, clave_hash));
    }

    abb_guardar(abb, clave_abb, vuelo1);
    hash_guardar(hash, clave_hash, vuelo2);

    free(clave_abb);
    free(clave_hash);
  }

  cola_destruir(cola, NULL);

  printf("OK\n");
  return true;
}

/*
 * Busca un vuelo dentro de un hash.
 * Pre: recibe la clave a ser buscado en el hash no nulo.
 * Post: imprime la información de vuelo y devuelve true en caso de que la
 * operación sea exitosa, false en caso contrario.
 */
bool info_vuelo(hash_t* hash, char* codigo) {
  char* buscado1 = (char*)hash_obtener(hash, codigo);
  if (buscado1 == NULL) return false;

  char** buscado2 = split(buscado1, SEPARADOR_COMA);
  if (buscado2 == NULL) return false;

  char* buscado3 = join(buscado2, SEPARADOR_ESPACIO);
  free_strv(buscado2);
  if (buscado3 == NULL) return false;

  printf("%s\n", buscado3);
  free(buscado3);
  printf("OK\n");
  return true;
}

/*
 * Recibe un abb y devuelve los k vuelos con más prioridad dentro de la misma.
 * Pre: el abb es válido y k es un número natural menor que la cantidad de
 * elementos del abb.
 * Post: utilizando un algorítmo de top-k y un heap de mínimos se imprimen los
 * k vuelos con mayor prioridad. Se devuelve true cuando la operación termina,
 * false en caso de error.
 */
bool prioridad(abb_t* abb, size_t k) {
  heap_t* minimos = heap_crear(cmp_prioridad);  // Creo un heap de mínimos
  if ((minimos == NULL) || (abb_cantidad(abb) < k)) return false;
  abb_iter_t* iter = abb_iter_in_crear(abb);
  if (iter == NULL) {
    heap_destruir(minimos, NULL);
    return false;
  }

  for (size_t i = 0; i < k; i++) {
    heap_encolar(minimos, abb_iter_in_ver_dato_actual(iter));
    abb_iter_in_avanzar(iter);
  }

  while (!abb_iter_in_al_final(iter)) {
    if (cmp_prioridad(abb_iter_in_ver_dato_actual(iter),
                      heap_ver_tope(minimos)) < 0) {
      heap_desencolar(minimos);
      heap_encolar(minimos, abb_iter_in_ver_dato_actual(iter));
    }
    abb_iter_in_avanzar(iter);
  }
  abb_iter_in_destruir(iter);

  char** lineas = malloc(sizeof(char*) * k);
  size_t i = k - 1;
  while (!heap_esta_vacio(minimos)) {
    lineas[i--] = (char*)heap_desencolar(minimos);
  }

  for (size_t j = 0; j < k; j++) {
    char** line = split(lineas[j], SEPARADOR_COMA);
    printf("%s - %s\n", line[5], line[0]);
    free_strv(line);
  }

  free(lineas);
  heap_destruir(minimos, NULL);
  printf("OK\n");
  return true;
}

/*
 * Imprime los k vuelos que se encuentran entre el rango de fechas dadas, de
 * forma ascendente o descendente.
 * Pre: recibe un árbol y fechas válidas. K debe ser menor que la cantidad de
 * vuelos que están en el rango seleccionado y obviamente menor que la cantidad
 * de vuelos totales.
 * Post: se imprimen los vuelos correspondientes y se devuelve true cuando la
 * operación termima exitosamente, false en caso contrario.
 */
bool ver_tablero(abb_t* abb, size_t k, char* type, char* desde, char* hasta) {
  if ((strcmp(type, ASCENDENTE) != 0 && strcmp(type, DESCENDENTE) != 0))
    return false;

  if (k > abb_cantidad(abb)) k = abb_cantidad(abb);

  if (fechas_incorrectas(desde, hasta)) return false;

  char** tablero_asc = obtener_rangos(abb, desde, hasta);
  size_t cantidad = strv_cantidad(tablero_asc, 1);
  size_t cont = 0;
  if (strcmp(type, DESCENDENTE) == 0) {
    size_t i = cantidad - 1;
    while (cont < k && tablero_asc[i] != NULL) {
      imprimir_tablero(desde, hasta, tablero_asc, i);
      i--;
      cont++;
    }
  } else {
    size_t i = 1;
    while (cont < k && tablero_asc[i] != NULL) {
      imprimir_tablero(desde, hasta, tablero_asc, i);
      i++;
      cont++;
    }
  }
  free(tablero_asc);
  printf("OK\n");

  return true;
}

/*
 * Borra los vuelos del sistema, es decir del hash y abb, que se encuentren en
 * el rango seleccionado.
 * Pre: recibe las fechas, un abb y hash válidos.
 * Post: elimina los vuelos correspondientes del sistema y devuelve true al
 * terminar bien el proceso, false en caso contrario.
 */
bool borrar(abb_t* abb, hash_t* hash, char* desde, char* hasta) {
  if (fechas_incorrectas(desde, hasta)) return false;

  char** vuelos_a_eliminar = obtener_rangos(abb, desde, hasta);
  int i = 1;

  while (vuelos_a_eliminar[i] != NULL) {
    char* clave_abb = generar_clave_abb(vuelos_a_eliminar[i]);
    char* clave_hash = generar_clave_hash(vuelos_a_eliminar[i]);
    if (clave_abb == NULL || clave_hash == NULL) {
      free(clave_abb);
      free(clave_hash);
      return false;
    }

    char** a_imprimir1 = split(vuelos_a_eliminar[i], SEPARADOR_COMA);
    char* a_imprimir2 = join(a_imprimir1, SEPARADOR_ESPACIO);
    printf("%s\n", a_imprimir2);
    free_strv(a_imprimir1);
    free(a_imprimir2);

    free(abb_borrar(abb, clave_abb));
    free(hash_borrar(hash, clave_hash));
    free(clave_abb);
    free(clave_hash);
    i++;
  }

  free(vuelos_a_eliminar);
  printf("OK\n");
  return true;
}

/******************************
 *                             *
 *          Programa           *
 *                             *
 ******************************/

/*
 * Crea las estructuras principales y da vida al sistema. Trabaja con
 * estructuras válidas y no nulas. Cargan los archivos que se le pasen y
 * opera con ellos. Terminará el programa cuando no queden más vuelos en el
 * sistema.
 * Post: ejecuta la operación correspondiente, en caso de terminar bien imprime
 * "OK", de lo contrario se imprime un error con mención al comando en el que
 * ocurrió el problema.
 */
int programa() {
  hash_t* hash = hash_crear(free);
  abb_t* abb = abb_crear(cmp_abb, free);

  if (hash == NULL || abb == NULL) {
    hash_destruir(hash);
    abb_destruir(abb);
    return ERROR_STATUS;
  }
  char* error = NULL;

  bool start = true;
  while (start || (hash_cantidad(hash) > 0 && abb_cantidad(abb) > 0)) {
    char string[TAM_MAX_INPUT];
    if (fgets(string, TAM_MAX_INPUT, stdin) == NULL) break;
    char** entrada = split(string, SEPARADOR_ESPACIO);
    error = NULL;

    if (strcmp(entrada[0], AGREGAR_ARCHIVO) == 0) {
      if (entrada[2] != NULL || !read_file(entrada[1], hash, abb))
        error = AGREGAR_ARCHIVO;
      else
        start = false;

    } else if (strcmp(entrada[0], VER_TABLERO) == 0) {
      if (!is_digit(entrada[1], strlen(entrada[1])) || entrada[5] != NULL ||
          !ver_tablero(abb, atoi(entrada[1]), entrada[2], entrada[3],
                       entrada[4]))
        error = VER_TABLERO;

    } else if (strcmp(entrada[0], INFO_VUELO) == 0) {
      if (strv_cantidad(entrada, 0) != 2 || entrada[2] != NULL ||
          !is_digit(entrada[1], strlen(entrada[1])) ||
          !info_vuelo(hash, entrada[1]))
        error = INFO_VUELO;

    } else if (strcmp(entrada[0], PRIORIDAD_VUELOS) == 0) {
      if (is_digit(entrada[1], strlen(entrada[1])) && atoi(entrada[1]) > 0) {
        if (entrada[2] != NULL || !prioridad(abb, (size_t)atoi(entrada[1])))
          error = PRIORIDAD_VUELOS;
      } else
        error = PRIORIDAD_VUELOS;

    } else if (strcmp(entrada[0], BORRAR) == 0) {
      if (strv_cantidad(entrada, 0) != 3 || entrada[3] != NULL ||
          !borrar(abb, hash, entrada[1], entrada[2]))
        error = BORRAR;
    }

    if (error != NULL) fprintf(stderr, "%s %s\n", ERROR, error);
    free_strv(entrada);
  }
  hash_destruir(hash);
  abb_destruir(abb);
  return OK_STATUS;
}

/*
 * Función principal, activa el resto.
 */
int main() {
  int status = programa();
  return status;
}
