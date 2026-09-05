#include <stdio.h>
#include <stdbool.h>
#include "tp1.h"

#define CANT_MIN_ARGC 2

#define ERROR -1
#define EXITO 0


int main (int agrc, char *argv[]) 
{

    if (agrc < CANT_MIN_ARGC) {
        return ERROR;
    }

    tp1_t *file_1;

    //Mando el nombre del archivo
    file_1 = tp1_leer_archivo(argv[1]);

    if (file_1 == NULL) {
        return ERROR;
    } else {
        printf("Se han cargado todos los datos de manera exitosa!\n");
    }
    
    printf("Cerrando el programa...\n\n");

    tp1_destruir(file_1);

    return EXITO;
}