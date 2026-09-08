#include "leer_linea.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define ERROR -1

char *leer_linea(FILE *archivo)
{
	if (archivo == NULL) {
		return NULL;
	}

	int size = 1;

	char *linea = NULL;
	linea = malloc((size_t)size * sizeof(char));

	if (linea == NULL) {
		return NULL;
	}

	//Leo el input mandado por consola
	int c = fgetc(archivo);

	if (c == EOF) {
		free(linea);
		return NULL;
	}

	bool mal_reservado = false;

	while (c != EOF && c != '\n' && !mal_reservado) {
		//Debo especificar que c tiene que ser un char
		linea[size - 1] = (char)c;
		size++;

		//Reservo memoría para el proximo loop
		char *aux = { NULL };

		aux = realloc(linea, (size_t)size);

		if (aux == NULL) {
			mal_reservado = true;
		} else {
			linea = aux;
			c = fgetc(archivo);
		}
	}

	if (mal_reservado) {
		free(linea);
		return NULL;
	}

	//Cierro el string
	linea[size - 1] = '\0';

	return linea;
}

int leer_linea_ptr(char **ptr, size_t *tamaño, FILE *archivo)
{
	//Verifico el puntero
	if (ptr == NULL || *ptr == NULL || archivo == NULL) {
		return ERROR;
	}

	size_t total_leidos = 1;

	int c = fgetc(archivo);

	bool mal_reservado = false;

	if (c == EOF) {
		return ERROR;
	}

	while (c != EOF && c != '\n' && !mal_reservado) {
		if (total_leidos >= *tamaño) {
			(*tamaño)++;

			char *aux = realloc(*ptr, (size_t)(*tamaño) + 1);

			if (aux == NULL) {
				mal_reservado = true;
			}

			if (!mal_reservado) {
				*ptr = aux;
			}
		}
		if (!mal_reservado) {
			(*ptr)[total_leidos - 1] = (char)c;
			c = fgetc(archivo);
			total_leidos++;
		}
	}

	if (mal_reservado) {
		return ERROR;
	}

	//Cierro el string
	(*ptr)[total_leidos - 1] = '\0';
	(*tamaño)++;

	//Corrijo el total con un -1
	return (int)total_leidos - 1;
}
