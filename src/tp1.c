#include <stdbool.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include "tp1.h"
#include "leer_linea.h"

#define PRESICION 0.000001
#define ERR 0
#define CAPACIDAD_INICIAL 10

//El %c extra, es precisamente un extra para hacer una lectura más precisa
const char *FOMRATO_LECTURA = "%m[^,],%d,%f,%c %c";
const char *FORMATO_ESCRITURA = "%s,%d,%.1f,%c\n";
const int LINEAS_ESPERADAS = 4;

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
	size_t capacidad_m;
};

char *duplicar_string(char *string)
{
	if (string == NULL) {
		return NULL;
	}

	size_t largo_string = strlen(string) + 1;

	char *nuevo_string = malloc(largo_string * sizeof(char));

	if (nuevo_string != NULL) {
		memcpy(nuevo_string, string, largo_string);
	}

	return nuevo_string;
}

void ordenar_pokemones(struct pokemon *pokemones, size_t tope)
{
	struct pokemon aux;
	size_t j;
	for (size_t i = 1; i < tope; i++) {
		j = i;
		aux = pokemones[i];
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

bool reservar_memoria_pokemon(struct pokemon **a_reservar, size_t capacidad)
{
	bool err = false;

	struct pokemon *aux =
		realloc(*a_reservar, sizeof(struct pokemon) * (capacidad));

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
	if (nombre == NULL) {
		return NULL;
	}

	FILE *archivo = fopen(nombre, MODO_LECTURA);

	if (archivo == NULL) {
		return NULL;
	}

	tp1_t *file = malloc(sizeof(tp1_t));

	if (file == NULL) {
		fclose(archivo);
		return NULL;
	}

	file->capacidad_m = CAPACIDAD_INICIAL;

	file->pokemones = malloc(sizeof(struct pokemon) * file->capacidad_m);

	if (file->pokemones == NULL) {
		free(file);
		fclose(archivo);
		return NULL;
	}

	file->cantidad = 0;

	int velocidad;
	float peso;
	char rareza;
	char otro;

	bool err = false;

	char *linea = leer_linea(archivo);

	while (linea != NULL && !err) {
		char *name = NULL;

		int leido = sscanf(linea, FOMRATO_LECTURA, &name, &velocidad,
				   &peso, &rareza, &otro);
		//Confirmo que sean 4 columnas
		if (leido == LINEAS_ESPERADAS) {
			bool repetido = false;

			for (int i = 0; i < file->cantidad && !repetido; i++) {
				if (strcasecmp(file->pokemones[i].nombre,
					       name) == 0) {
					repetido = true;
				}
			}

			if (!repetido) {
				if (file->cantidad >= file->capacidad_m) {
					file->capacidad_m *= 2;
					err = reservar_memoria_pokemon(
						&file->pokemones,
						file->capacidad_m);
				}

				if (!err) {
					bool data = cargar_tp1(file, velocidad,
							       peso, name,
							       rareza);
					if (data) {
						file->cantidad++;
					} else { // Si se cargaron mal los datos
						free(name);
					}
				} else { //Si hubo un error al reservar más memoria
					free(name);
				}
			} else { //Si es un pokemon repetido
				free(name);
			}
		} else { //Si se leyeron líneas distintas a 4
			if (leido > 0) {
				free(name);
			}
		}
		//Ya cargué el dato por ende libero el "linea"
		free(linea);
		linea = leer_linea(archivo);
	}

	if (err) {
		tp1_destruir(file);
		fclose(archivo);
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

	tp1_r->capacidad_m = tp1_a->cantidad + tp1_b->cantidad;
	tp1_r->pokemones = malloc(sizeof(struct pokemon) * tp1_r->capacidad_m);

	if (tp1_r->pokemones == NULL) {
		return NULL;
	}

	tp1_r->cantidad = 0;

	int i_a = 0;
	int i_b = 0;

	bool err = false;

	ordenar_pokemones(tp1_b->pokemones, tp1_b->cantidad);

	ordenar_pokemones(tp1_a->pokemones, tp1_a->cantidad);

	while (i_a < tp1_a->cantidad && i_b < tp1_b->cantidad && !err) {
		int comp = strcasecmp(tp1_a->pokemones[i_a].nombre,
				      tp1_b->pokemones[i_b].nombre);

		bool valid = true;
		//Caso: Primer string mayor (o sea tp1_a)
		if (comp > 0) {
			char *nuevo_nombre =
				duplicar_string(tp1_b->pokemones[i_b].nombre);

			if (nuevo_nombre == NULL) {
				valid = false;
			} else {
				tp1_r->pokemones[tp1_r->cantidad] =
					tp1_b->pokemones[i_b];
				tp1_r->pokemones[tp1_r->cantidad].nombre =
					nuevo_nombre;
				i_b++;
			}
		} else if (comp < 0) { //Caso: Primer string menor
			char *nuevo_nombre =
				duplicar_string(tp1_a->pokemones[i_a].nombre);

			if (nuevo_nombre == NULL) {
				valid = false;
			} else {
				tp1_r->pokemones[tp1_r->cantidad] =
					tp1_a->pokemones[i_a];
				tp1_r->pokemones[tp1_r->cantidad].nombre =
					nuevo_nombre;
				i_a++;
			}

		} else { //Caso: Son iguales
			//Solamente cargo el primero (solicitado por el .h)
			char *nuevo_nombre =
				duplicar_string(tp1_a->pokemones[i_a].nombre);

			if (nuevo_nombre == NULL) {
				valid = false;
			} else {
				tp1_r->pokemones[tp1_r->cantidad] =
					tp1_a->pokemones[i_a];

				tp1_r->pokemones[tp1_r->cantidad].nombre =
					nuevo_nombre;

				i_a++;
				i_b++;
			}
		}

		if (valid) {
			tp1_r->cantidad++;
		}
	}

	//Agrego al vector tp1_r lo que restó, si es así, del tp1_a
	while (i_a < tp1_a->cantidad && !err) {
		char *nuevo_nombre =
			duplicar_string(tp1_a->pokemones[i_a].nombre);

		bool valid = true;

		if (nuevo_nombre == NULL) {
			valid = false;
		} else {
			tp1_r->pokemones[tp1_r->cantidad] =
				tp1_a->pokemones[i_a];
			tp1_r->pokemones[tp1_r->cantidad].nombre = nuevo_nombre;
			i_a++;
		}

		if (valid) {
			tp1_r->cantidad++;
		}
	}

	//Agrego al vector tp1_r lo que restó, si es así, del tp1_b
	while (i_b < tp1_b->cantidad && !err) {
		char *nuevo_nombre =
			duplicar_string(tp1_b->pokemones[i_b].nombre);

		bool valid = true;

		if (nuevo_nombre == NULL) {
			valid = false;
		} else {
			tp1_r->pokemones[tp1_r->cantidad] =
				tp1_b->pokemones[i_b];
			tp1_r->pokemones[tp1_r->cantidad].nombre = nuevo_nombre;
			i_b++;
		}

		if (valid) {
			tp1_r->cantidad++;
		}
	}

	if (err) {
		tp1_destruir(tp1_r);
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
			tp1->pokemones[i].velocidad,
			tp1->pokemones[i].peso + PRESICION, rareza_c);
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
	if (tp1 == NULL || nombre == NULL) {
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
	//Por si el usuario pasa n >= tope
	if (tp1 == NULL || n >= tp1->cantidad) {
		return NULL;
	}

	ordenar_pokemones(tp1->pokemones, tp1->cantidad);

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

	if (tp1 == NULL || f == NULL || extra == NULL) {
		return contador;
	}

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
