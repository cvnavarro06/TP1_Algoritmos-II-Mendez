#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "tp1.h"
#include "leer_linea.h"

const char *FOMRATO_LECTURA = "%m[^,],%d,%f,%c";
const char *FORMATO_ESCRITURA = "%s,%d,%f,%c\n";

const char *MODO_LECTURA = "r";
const char *MODO_ESCRITURA = "w";

const int MAX_VELOCIDAD = 99;
const int MIN_VELOCIDAD = 1;

const char RAREZA_COMUN_C = 'C';
const char RAREZA_RARO_C = 'R';
const char RAREZA_LEGENDARIO_C = 'L';

struct tp1 {
    struct pokemon *pokemones;
    int cantidad;
};

bool cargar_tp1(tp1_t *tp1, int velocidad, float peso, char *nombre, char rareza)
{

    bool invalid_data = false;

    if (velocidad >= MIN_VELOCIDAD && velocidad <= MAX_VELOCIDAD) {
            tp1->pokemones[tp1->cantidad].velocidad = velocidad;
    } else {
        invalid_data = true;
    }
    
    if (peso > 0) {
        tp1->pokemones[tp1->cantidad].peso = peso;
    } else {
        invalid_data = true;
    }

    if (rareza == RAREZA_COMUN_C) {
        tp1->pokemones[tp1->cantidad].rareza = RAREZA_COMUN;
    } else if (rareza == RAREZA_RARO_C) {
        tp1->pokemones[tp1->cantidad].rareza = RAREZA_RARO;
    } else if (rareza == RAREZA_LEGENDARIO_C) {
        tp1->pokemones[tp1->cantidad].rareza = RAREZA_LEGENDARIO;
    } else {
        invalid_data = true;
    }
    
    if (!invalid_data) tp1->pokemones[tp1->cantidad].nombre = nombre;

    return invalid_data;
    
}



tp1_t *tp1_leer_archivo(const char *nombre)
{

    FILE *archivo = fopen(nombre, MODO_LECTURA);

    if (archivo == NULL) {
        return NULL;
    }

    tp1_t *file = malloc(sizeof(tp1_t));

    if (file == NULL) {
        return NULL;
    }

    //Inicialzo datos
    file->pokemones = malloc(sizeof(struct pokemon));

    if (file->pokemones == NULL) {
        return NULL;
    }

    file->cantidad = 0;

    char* name;
    int velocidad;
    float peso;
    char rareza;

    bool err = false;

    char *linea = leer_linea(archivo); 

    while (linea != NULL && !err) {
        int leido = sscanf(linea, FOMRATO_LECTURA, name, &velocidad, &peso, &rareza);
        //Confirmo que sean 4 columnas
        if (leido == 4) {
            bool repetido = false;

            for (int i = 0; i<file->cantidad; i++) {
                if (strcasecmp(file->pokemones[i].nombre, name) == 0) {
                    repetido = true;
                }
            }
            if (!repetido) {
                bool data = cargar_tp1(file, velocidad, peso, name, rareza);

                if (!data) {
                    file->cantidad++;
                    struct pokemon *aux = realloc(file->pokemones, sizeof(struct pokemon) * (size_t)(file->cantidad + 1));

                    if (aux == NULL) {
                        err = true;
                    } else {
                        file->pokemones = aux;
                    }
                }
            } else {
                free(name);
            }

        }
        //Ya cargué el dato por ende libero el "string"
        free(linea);
        linea = leer_linea(archivo);
    }

    if (err) {
        return NULL;
    }

    fclose(archivo);

    return file;
}


size_t tp1_cantidad(tp1_t *tp1);


tp1_t *tp1_combinar(tp1_t *tp1_a, tp1_t *tp1_b);


tp1_t *tp1_escribir_archivo(tp1_t *tp1, const char *nombre);


struct pokemon *tp1_buscar_pokemon(tp1_t *tp1, const char *nombre);


struct pokemon *tp1_buscar_orden(tp1_t *tp1, size_t n);

size_t tp1_iterar(tp1_t *tp1, bool (*f)(struct pokemon *, void *), void *extra);


tp1_t *tp1_destruir(tp1_t *tp1);