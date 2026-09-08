#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "tp1.h"
#include "leer_linea.h"

#define PRESICION 0.000001
#define ERR 0

const char *FOMRATO_LECTURA = "%m[^,],%d,%f,%c";
const char *FORMATO_ESCRITURA = "%s,%d,%.1f,%c\n";

const char *MODO_LECTURA = "r";
const char *MODO_ESCRITURA = "w";

const int MAX_VELOCIDAD = 99;
const int MIN_VELOCIDAD = 1;
const int PESO_MIN = 0;

const char RAREZA_COMUN_C = 'C';
const char RAREZA_RARO_C = 'R';
const char RAREZA_LEGENDARIO_C = 'L';



struct tp1 {
	struct pokemon *pokemones;
	size_t cantidad;
};

void ordenar_pokemones(struct pokemon *pokemones, size_t tope)
{
	struct pokemon aux;
	size_t j;
	for (size_t i = 1; i < tope; i++) {
		j = i;
		aux = pokemones[i];
		printf("He entrado al for\n");
		while ((j > 0) &&
		       strcasecmp(pokemones[j - 1].nombre, aux.nombre) > 0) {
			pokemones[j] = pokemones[j - 1];
			j--;
		}
		pokemones[j] = aux;
	}
}

bool cargar_tp1(tp1_t *tp1, int velocidad, float peso, char *nombre,
		char rareza)
{

	if (velocidad >= MIN_VELOCIDAD && velocidad <= MAX_VELOCIDAD) {
		tp1->pokemones[tp1->cantidad].velocidad = velocidad;
	} else {
		return false;
	}

	if (peso > PESO_MIN) {
		tp1->pokemones[tp1->cantidad].peso = peso;
	} else {
		return false;
	}

	if (rareza == RAREZA_COMUN_C) {
		tp1->pokemones[tp1->cantidad].rareza = RAREZA_COMUN;
	} else if (rareza == RAREZA_RARO_C) {
		tp1->pokemones[tp1->cantidad].rareza = RAREZA_RARO;
	} else if (rareza == RAREZA_LEGENDARIO_C) {
		tp1->pokemones[tp1->cantidad].rareza = RAREZA_LEGENDARIO;
	} else {
		return false;
	}

	tp1->pokemones[tp1->cantidad].nombre = nombre;

	return true;
}

bool reservar_memoria(struct pokemon **a_reservar, size_t tamanio)
{
	bool err = false;

	struct pokemon *aux = realloc(*a_reservar, sizeof(struct pokemon) * (tamanio + 1));

	if (aux == NULL) {
		err = true;
	} else {
		*a_reservar = aux;
	}

	return err;
}

/**
 * Lee un archivo de pokemon según especificado en el enunciado y devuelve un TP1 con dichos pokemon.
 *
 * En caso de error devuelve NULL
 */
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
		int leido = sscanf(linea, FOMRATO_LECTURA, &name, &velocidad,
				   &peso, &rareza);
		//Confirmo que sean 4 columnas
		if (leido == 4) {
            
            bool repetido = false;
            
			for (int i = 0; i < file->cantidad; i++) {
                if (strcasecmp(file->pokemones[i].nombre,
                    name) == 0) {
                    repetido = true;
                }
            }

            if (!repetido) {
                bool data = cargar_tp1(file, velocidad, peso,
						       name, rareza);

							   
				if (data) {
					file->cantidad++;
					err = reservar_memoria(
						&file->pokemones,
						file->cantidad);
				} else {
					free(name);
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

/**
 * Devuelve la cantidad de pokemon leidos en este TP1
 *
 * En caso de error devuelve 0
 *
 */
size_t tp1_cantidad(tp1_t *tp1)
{
	if (tp1 == NULL) {
		return ERR;
	}

	return tp1->cantidad;
}

/**
 * Crea un TP1 con la combinación de los pokemon de ambos TPs. En caso de
 * duplicados se toma el pokemon del primer TP1.
 *
 * Devuelve un nuevo TP1 con la combinación de ambos TP1 o NULL en caso de
 * error.
 *
 */
tp1_t *tp1_combinar(tp1_t *tp1_a, tp1_t *tp1_b)
{
	if (tp1_a == NULL || tp1_b == NULL) {
		return NULL;
	}

	//Reservo un tp1
	tp1_t *tp1_r = malloc(sizeof(tp1_t));
	

	if (tp1_r == NULL) {
		return NULL;
	}

	printf("Reservo memoria para tp1\n");

	tp1_r->pokemones = malloc(sizeof(struct pokemon));

	if (tp1_r->pokemones == NULL) {
		return NULL;
	}

	printf("Reservo memoria para pokemon\n");

	tp1_r->cantidad = 0;

	int i_a = 0;
	int i_b = 0;

	bool err = false;

	printf("Previo a ordenar\n");

	ordenar_pokemones(tp1_b->pokemones, tp1_b->cantidad);

	printf("tp1_b Ordenado\nYendo a ordenar el tp1_a\n");

	ordenar_pokemones(tp1_a->pokemones, tp1_a->cantidad);

	printf("Ordenado correctamente");

	while (i_a < tp1_a->cantidad && i_b < tp1_b->cantidad && !err) {
		int comp = strcasecmp(tp1_a->pokemones[i_a].nombre,
				      tp1_b->pokemones[i_b].nombre);

		bool data;

		//Caso: Primer string mayor (o sea tp1_a)
		if (comp > 0) {
			data = cargar_tp1(tp1_r,
					  tp1_b->pokemones[i_b].velocidad,
					  tp1_b->pokemones[i_b].peso,
					  tp1_b->pokemones[i_b].nombre,
					  tp1_b->pokemones[i_b].rareza);
			i_b++;

			if (!data) {
				err = reservar_memoria(
					&tp1_r->pokemones,
					tp1_r->cantidad);
			}
		} else if (comp < 0) { //Caso: Primer string menor
			data = cargar_tp1(tp1_r,
					  tp1_a->pokemones[i_a].velocidad,
					  tp1_a->pokemones[i_a].peso,
					  tp1_a->pokemones[i_a].nombre,
					  tp1_a->pokemones[i_a].rareza);
			i_a++;

			if (!data) {
				err = reservar_memoria(
					&tp1_r->pokemones,
					tp1_r->cantidad);
			}
		} else { //Caso: Son iguales
			//Solamente cargo 1 de los 2 (Se unen)
			data = cargar_tp1(tp1_r,
					  tp1_a->pokemones[i_a].velocidad,
					  tp1_a->pokemones[i_a].peso,
					  tp1_a->pokemones[i_a].nombre,
					  tp1_a->pokemones[i_a].rareza);

			i_a++;
			i_b++;

			if (!data) {
				tp1_r->cantidad++;
				err = reservar_memoria(
					&tp1_r->pokemones,
					tp1_r->cantidad);
			}
		}
	}

	if (err) {
		return NULL;
	}

	while (i_a < tp1_a->cantidad) {
		bool data = cargar_tp1(tp1_r, tp1_a->pokemones[i_a].velocidad,
				       tp1_a->pokemones[i_a].peso,
				       tp1_a->pokemones[i_a].nombre,
				       tp1_a->pokemones[i_a].rareza);
		i_a++;

		if (!data) {
			err = reservar_memoria(&tp1_r->pokemones,
					 tp1_r->cantidad);
		}
	}

	while (i_b < tp1_b->cantidad) {
		bool data = cargar_tp1(tp1_r, tp1_b->pokemones[i_b].velocidad,
				       tp1_b->pokemones[i_b].peso,
				       tp1_b->pokemones[i_b].nombre,
				       tp1_b->pokemones[i_b].rareza);
		i_b++;

		if (!data) {
			err = reservar_memoria(&tp1_r->pokemones,
					 tp1_r->cantidad);
		}
	}

	if (err) {
		return NULL;
	}

	return tp1_r;
}

/**
 * Guarda el TP1 a un archivo de texto.
 *
 * Devuelve el mismo TP1 o NULL en caso de error. Si el archivo ya existe se
 * sobreescribe.
 *
 */
tp1_t *tp1_escribir_archivo(tp1_t *tp1, const char *nombre)
{
	if (tp1 == NULL) {
		return NULL;
	}

	FILE *archivo = fopen(nombre, MODO_ESCRITURA);

	if (archivo == NULL) {
		return NULL;
	}

	for (int i = 0; i < tp1->cantidad; i++) {

		char rareza_c = RAREZA_COMUN_C;

		int rareza_e = tp1->pokemones[i].rareza;

		if (rareza_e == RAREZA_COMUN) {
			rareza_c = RAREZA_COMUN_C;
		} else if (rareza_e == RAREZA_RARO) {
			rareza_c = RAREZA_RARO_C;
		} else if (rareza_e == RAREZA_LEGENDARIO) {
			rareza_c = RAREZA_LEGENDARIO_C;
		}

		fprintf(archivo, FORMATO_ESCRITURA, tp1->pokemones[i].nombre,
			tp1->pokemones[i].velocidad, tp1->pokemones[i].peso + PRESICION,
			rareza_c);
	}

	fclose(archivo);

	return tp1;
}

/**
 * Busca un pokemon por nombre dentro del TP1 y lo devuelve.
 *
 * En caso de error devuelve NULL
 */
struct pokemon *tp1_buscar_pokemon(tp1_t *tp1, const char *nombre)
{
	if (tp1 == NULL) {
		return NULL;
	}

	int i_encontrado;

	bool encontrado = false;

	for (int i = 0; i < tp1->cantidad; i++) {
		if (strcasecmp(tp1->pokemones[i].nombre, nombre) == 0) {
			i_encontrado = i;
			encontrado = true;
		}
	}

	if (!encontrado) {
		return NULL;
	}

	return &tp1->pokemones[i_encontrado];
}

/**
 * Busca el n-esimo pokemon del TP1 (ordenados por orden alfabético)
 *
 * En caso de error devuelve NULL
 */
struct pokemon *tp1_buscar_orden(tp1_t *tp1, size_t n)
{
	if (tp1 == NULL) {
		return NULL;
	}

	ordenar_pokemones(tp1->pokemones, tp1->cantidad);

	printf("file ordenado correctamente\n");

	return &tp1->pokemones[n];
}

/**
 * Recorre los pokemon del TP1 aplicando la función f a cada uno. Si f devuelve
 * true, sigue iterando.
 *
 * Devuelve la cantidad de veces que se invocó f.
 */
size_t tp1_iterar(tp1_t *tp1, bool (*f)(struct pokemon *, void *), void *extra)
{
	size_t contador = 0;

	for (int i = 0; i < tp1->cantidad; i++) {
		if ((f)(&tp1->pokemones[i], extra)) {
			contador++;
		}
	}

	return contador;
}

/**
 * Destruye el TP1 y toda la memoria asociada
 *
 */
void *tp1_destruir(tp1_t *tp1)
{
	//Analizar luego esta función
	if (tp1 == NULL) {
		return NULL;
	}

	for (int i = 0; i < tp1->cantidad; i++) {
		free(tp1->pokemones[i].nombre);
	}

	free(tp1->pokemones);
	free(tp1);

    return NULL;
}
