#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>
#include "strutil.h"

/*
 * Devuelve una nueva cadena con los primeros ‘n’ caracteres de la cadena
 * ‘str’. La liberación de la memoria dinámica devuelta queda a cargo de
 * quien llame a esta función.
 *
 * Devuelve NULL si no se pudo reservar suficiente memoria.
 */
 char *substr(const char *str, size_t n){
     if (str == NULL) return NULL;
     char* cadena = malloc(sizeof(char)*(n+1));
     if (n != 0) strncpy(cadena, str, n);
     cadena[n] = '\0';
     return cadena;
 }

/*
 * Devuelve el número que corresponde con la cantidad de veces
 * que el separador aparece en el string.
 *
 * Devuelve 1 si el string es nulo.
 */
 size_t contador_de_separadores(const char* str, char sep, size_t largo){
     size_t contador_de_sep = 0;
     for (size_t i = 0;i < largo;i++){
         if (str[i] == sep) contador_de_sep++;
     }
     return contador_de_sep;
 }

 char** split(const char *str, char sep){
     if (str == NULL) return NULL;
     size_t largo = strlen(str);
     size_t contador_de_sep = contador_de_separadores(str,sep,largo);
     char** strv = malloc(sizeof(char*)*(contador_de_sep+2));
     if (strv == NULL) return NULL;
     size_t pos_en_vector = 0;
     size_t ult_aparicion = 0;
     for(size_t contador = 0;contador <= largo;contador++){
         if (str[contador] == sep || str[contador] == '\0'){
             char* word = substr(str + ult_aparicion,contador - ult_aparicion);
             if (word == NULL){
             free_strv(strv);
                 return NULL;
             }
             ult_aparicion = contador+1;
             strv[pos_en_vector++] = word;
         }
     }
     if (strv[pos_en_vector - 1][strlen(strv[pos_en_vector - 1])-1] == '\n'){
         char* reemplazo = substr(strv[pos_en_vector - 1], strlen(strv[pos_en_vector - 1])-1);//PARA EVITAR EL /n
         free(strv[pos_en_vector - 1]);
         strv[pos_en_vector - 1] = reemplazo;
     }
     strv[pos_en_vector] = NULL;
     return strv;
 }


/*
 * Libera un arreglo dinámico de cadenas, y todas las cadenas que contiene.
 */
 void free_strv(char *strv[]){
     if (strv == NULL) return;
     int pos = 0;
     while (strv[pos] != NULL){
         free(strv[pos]);
         pos++;
     }
     free(strv[pos]);
     free(strv);
 }

/*
 * Devuelve la cadena resultante de unir todas las cadenas del arreglo
 * terminado en NULL ‘str’ con ‘sep’ entre cadenas. La cadena devuelta se
 * ubicará en un nuevo espacio de memoria dinámica.
 *
 * La liberación de la memoria dinámica devuelta queda a cargo de quien llame a
 * esta función. La función devuelve NULL en caso de error.
 */
 char *join(char **strv, char sep){
     if (strv == NULL) return ("");
     size_t contador = 0;
     size_t pos_en_vector = 0;
     size_t palabras_a_juntar = 0;
     size_t pos = 0;
     size_t tamanio = 1;
     while(strv[contador] != NULL){
         tamanio += strlen(strv[contador++]) + 1;
         palabras_a_juntar++;
     }
     if (strv[0] != NULL) tamanio--;
     char* palabra = calloc(sizeof(char),tamanio);
     if (palabra == NULL) return NULL;
     for(contador = 0;contador < palabras_a_juntar;contador++){
         if (strv[pos_en_vector] != NULL){
             strcat(palabra,strv[pos_en_vector]);
             pos += strlen(strv[pos_en_vector]);
         }
         palabra[pos] = sep;
         pos++;
         pos_en_vector++;
     }
     palabra[tamanio-1] = '\0';
     return palabra;
 }
