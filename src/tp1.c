#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "tp1.h"
#include "leer_linea.h"

#define ERR 0

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
    size_t cantidad;
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

bool reservar_memoria(void** a_reservar, size_t tamanio) 
{
    bool err = false;

    void *aux = realloc(a_reservar, tamanio + 1);

    if (aux == NULL) {
        err = false;
    } else {
        *a_reservar = aux;
    }

    return err;
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

    file->pokemones = malloc(sizeof(struct pokemon));

    if (file->pokemones == NULL) {
        return NULL;
    }

    file->cantidad = 0;

    char *name;
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
                    struct pokemon *aux = realloc(file->pokemones, sizeof(struct pokemon) * (file->cantidad + 1));

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


size_t tp1_cantidad(tp1_t *tp1)
{
    if (tp1 == NULL) {
        return ERR;
    }

    return tp1->cantidad;

}

tp1_t *tp1_combinar(tp1_t *tp1_a, tp1_t *tp1_b)
{
    if (tp1_a == NULL && tp1_b == NULL) {
        return NULL;
    }

    //Reservo un tp1
    tp1_t *tp1_r = malloc(sizeof(tp1_t));

    if (tp1_r == NULL) {
        return NULL;
    }

    tp1_r->pokemones = malloc(sizeof(struct pokemon));

    if (tp1_r->pokemones == NULL) {
        return NULL;
    }

    tp1_r->cantidad = 0;

    int i_a = 0;
    int i_b = 0;

    bool err = false;

    while (i_a <= tp1_a->cantidad && i_b <= tp1_b->cantidad && !err) {

        int comp = strcasecmp(tp1_a->pokemones[i_a].nombre, tp1_b->pokemones[i_b].nombre);
        
        bool data;
        //Caso: Primer string mayor (o sea tp1_a)
        if (comp > 0) {
            data = cargar_tp1(tp1_r, tp1_b->pokemones[i_b].velocidad, tp1_b->pokemones[i_b].peso,
                                   tp1_b->pokemones[i_b].nombre, tp1_b->pokemones[i_b].rareza);
            i_b++;

            if (!data) {
                reservar_memoria((void**)&tp1_r->pokemones, tp1_r->cantidad);
            }
            
        
        } else if (comp < 0) { //Caso: Primer string menor
            data = cargar_tp1(tp1_r, tp1_a->pokemones[i_a].velocidad, tp1_a->pokemones[i_a].peso,
                                   tp1_a->pokemones[i_a].nombre, tp1_a->pokemones[i_a].rareza);
            i_a++;
            
            if (!data) {
                reservar_memoria((void**)&tp1_r->pokemones, tp1_r->cantidad);
            }
        } else { //Caso: Son iguales
            //Solamente cargo 1 de los 2 (Se unen)
            data = cargar_tp1(tp1_r, tp1_b->pokemones[i_b].velocidad, tp1_b->pokemones[i_b].peso,
                              tp1_b->pokemones[i_b].nombre, tp1_b->pokemones[i_b].rareza);
            
            i_a++;
            i_b++;

            if (!data) {
                reservar_memoria((void**)&tp1_r->pokemones, tp1_r->cantidad);
            }

        }

        tp1_r->cantidad++;
    }

    
    return tp1_r;
}

tp1_t *tp1_escribir_archivo(tp1_t *tp1, const char *nombre)
{
    if (tp1 == NULL) {
        return NULL;
    }

    FILE *archivo = fopen(nombre, MODO_ESCRITURA);

    if (archivo == NULL) {
        return NULL;
    }

    for (int i = 0; i<tp1->cantidad; i++) {
        fprintf(archivo, FORMATO_ESCRITURA, tp1->pokemones[i].nombre, tp1->pokemones[i].velocidad,
                tp1->pokemones[i].peso, tp1->pokemones[i].rareza);
    }

    fclose(archivo);

    return tp1;
}

struct pokemon *tp1_buscar_pokemon(tp1_t *tp1, const char *nombre)
{
    if (tp1 == NULL) {
        return NULL;
    }
    
    int i_encontrado;

    bool encontrado = false;

    for (int i = 0; i<tp1->cantidad; i++) {
        if (strcasecmp(tp1->pokemones[i].nombre, nombre) == 0) {
            i_encontrado = i;
            encontrado = true;
        }
    }

    if (!encontrado) {
        return NULL;
    }

    return tp1->pokemones + i_encontrado;

}


struct pokemon *tp1_buscar_orden(tp1_t *tp1, size_t n)
{
    if (tp1 == NULL || n < 0) {
        return NULL;
    }

    //Función de ordenamiento, analizar luego la menos "costosa"

    return tp1->pokemones + n;
}

size_t tp1_iterar(tp1_t *tp1, bool (*f)(struct pokemon *, void *), void *extra);


void *tp1_destruir(tp1_t *tp1)
{
    //Analizar luego esta función
    if (tp1 == NULL) {
        return NULL;
    }

    for (int i = 0; i<tp1->cantidad; i++) {
        free(tp1->pokemones[i].nombre);
        //free(tp1->pokemones[i]);
    }

    free(tp1);
}