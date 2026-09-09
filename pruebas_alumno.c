#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "tp1.h"
#include "leer_linea.h"
#include "pa2m.h"

#define ARCHIVO_CARGADO "files_csv/pokedex.csv"
#define ARCHIVO_CARGADO_1 "files_csv/pokedex1.csv"
#define ARCHIVO_CON_ERRORES "files_csv/pokedex_err.csv"
#define ARCHIVO_VACIO "files_csv/empty.csv"
#define ARCHIVO_INEXISTENTE "files_csv/null.csv"
#define ARCHIVO_NUEVO "files_csv/nuevo_archivo.csv"

const int POKEMONES_ERR_LINEA = 4;
const int POKEMONES_ESPERADOS = 5;

const char *POKEMON_BUSCADO = "Pikachu";
const char *POKEMON_BUSCADO1 = "Magikarp";
const char *POKEMON_CASE_SENSITIVE = "buLbAsAuR";
const char *POKEMON_INEXISTENTE = "NoExiste";

const size_t FUERA_DE_RANGO = 100;



struct tp1
{
    struct pokemon *pokemones;
    size_t cantidad;
};

#define EXITO 0

bool buscando_un_pokemon(struct pokemon *pokemon, void *extra)
{   
    bool encontrado = false;

    char *pokemon_a_buscar = (char*)extra;

    if (strcasecmp(pokemon->nombre, extra) == 0) {
        encontrado = true;
    }

    return encontrado;
}

bool buscar_misma_velocidad (struct pokemon *pokemon, void *extra)
{
    bool mismo_peso = false;

    int *velocidad_a_buscar = (int*)extra;

    if (pokemon->velocidad == *velocidad_a_buscar) {
        mismo_peso = true;
    }

    return mismo_peso;
}

bool iterar_siempre(struct pokemon *pokemon, void *extra)
{
	return true;
}

//Testeos de lectura
void lectura_error() 
{
    tp1_t *file = tp1_leer_archivo(ARCHIVO_CON_ERRORES);

    pa2m_afirmar(file != NULL, "Lectura de un archivo con un error de tipeo en una línea no devuelve NULL");
    
    tp1_destruir(file);
}

void lectura_vacio_e_inexistente()
{
    tp1_t *file = tp1_leer_archivo(ARCHIVO_VACIO);

    pa2m_afirmar(file != NULL, "Lectura de un archivo vacío devuelve un tp1 válido");

    tp1_destruir(file);

    tp1_t *file_1 = tp1_leer_archivo(ARCHIVO_INEXISTENTE);
    
    pa2m_afirmar(file_1 == NULL, "Lectura de un archivo inexistente devuelve NULL");
    
    tp1_destruir(file_1);
}

void lectura_correcta()
{
    tp1_t *file = tp1_leer_archivo(ARCHIVO_CARGADO);

    pa2m_afirmar(file != NULL, "La lectura de un archivo cargado y devuelve una estructura de datos cargada con los datos del archivo mismo");
    
    tp1_destruir(file);
}

//Testeos de cantidades
void cantidades() 
{
    tp1_t *file = tp1_leer_archivo(ARCHIVO_CARGADO);

    pa2m_afirmar(tp1_cantidad(file) == POKEMONES_ESPERADOS, "Un tp1 cargado con un archivo que contiene 5 pokemones, devuelve 5 pokemones");

    tp1_destruir(file);

    tp1_t *file_1 = tp1_leer_archivo(ARCHIVO_CON_ERRORES);

    pa2m_afirmar(tp1_cantidad(file_1) == POKEMONES_ERR_LINEA, "Un archivo con 5 pokemones pero una línea está mal tipeada, tp1 carga 4 pokemones");

    tp1_destruir(file_1);

    tp1_t *file_2 = tp1_leer_archivo(ARCHIVO_VACIO);

    pa2m_afirmar(tp1_cantidad(file_2) == 0, "Lectura de un archivo vacío carga 0 pokemones a tp1");

    tp1_destruir(file_2);

} 

//Testeos de escritura de archivos
void escribir_archivo_null()
{
    pa2m_afirmar(tp1_escribir_archivo(NULL, ARCHIVO_NUEVO) == NULL,
		    "Un tp1 con valor NULL devuelve NULL");
}

void escribir_archivo_exito()
{
	tp1_t *file = tp1_leer_archivo(ARCHIVO_CARGADO);
	pa2m_afirmar(tp1_escribir_archivo(file, ARCHIVO_NUEVO) != NULL,
		    "Guardar un tp1 cargado y luego se genera un archivo nuevo correctamente");

	tp1_t *file_escrito = tp1_leer_archivo(ARCHIVO_NUEVO);
	pa2m_afirmar(tp1_cantidad(file_escrito) == tp1_cantidad(file),
		    "El archivo escrito y vuelto a leer mantiene la misma cantidad de pokemones");

	tp1_destruir(file);
	tp1_destruir(file_escrito);
}

//Testeos de combinaciones
void combinaciones_con_null()
{
    tp1_t *file = tp1_leer_archivo(ARCHIVO_CARGADO);
	tp1_t *file_1 = tp1_leer_archivo(ARCHIVO_CARGADO_1);

	pa2m_afirmar(tp1_combinar(NULL, file_1) == NULL,
		    "Combinar con el primer tp1 NULL devuelve NULL");
	pa2m_afirmar(tp1_combinar(file, NULL) == NULL,
		    "Combinar con el segundo tp1 NULL devuelve NULL");

    tp1_destruir(file);
	tp1_destruir(file_1);
}

void combinar_tp_vacio()
{
    tp1_t *file = tp1_leer_archivo(ARCHIVO_CARGADO);
	tp1_t *file_vacio = tp1_leer_archivo(ARCHIVO_VACIO);


	tp1_t *combinado_vacio = tp1_combinar(file, file_vacio);
	pa2m_afirmar(tp1_cantidad(combinado_vacio) == tp1_cantidad(file),
		    "Combinar un tp1 cargado con uno vacío mantiene la cantidad original del tp1 cargado");
	tp1_destruir(combinado_vacio);
    tp1_destruir(file);
    tp1_destruir(file_vacio);

}

void combinar_exito()
{
	tp1_t *file_a = tp1_leer_archivo(ARCHIVO_CARGADO);
	tp1_t *file_b = tp1_leer_archivo(ARCHIVO_CARGADO_1);

	tp1_t *combinado = tp1_combinar(file_a, file_b);
	pa2m_afirmar(combinado != NULL, "Combinar dos tp1 válidos devuelve un nuevo tp1");
	pa2m_afirmar(tp1_cantidad(combinado) >= tp1_cantidad(file_a),
		    "La cantidad del tp1 combinado es mayor o igual a la del primero");

	tp1_destruir(file_a);
	tp1_destruir(file_b);
	tp1_destruir(combinado);
}

//Testeos de buscar pokemones
void buscar_pokemon_null()
{
    tp1_t *file = tp1_leer_archivo(ARCHIVO_CARGADO_1);

	pa2m_afirmar(tp1_buscar_pokemon(NULL, POKEMON_BUSCADO) == NULL,
		    "Buscar en un tp1 NULL devuelve NULL");
	pa2m_afirmar(tp1_buscar_pokemon(file, NULL) == NULL,
		    "Buscar un nombre NULL devuelve NULL");
    
    tp1_destruir(file);
}

void buscar_pokemon_especifico()
{
    tp1_t *file = tp1_leer_archivo(ARCHIVO_CARGADO);

	struct pokemon *p1 = tp1_buscar_pokemon(file, POKEMON_BUSCADO1);
	pa2m_afirmar(p1 != NULL, "Buscar un pokemon existente devuelve un puntero valido al pokemon buscado");
    
	tp1_destruir(file);
}

void buscar_pokemon_case_sensitive()
{
	tp1_t *file = tp1_leer_archivo(ARCHIVO_CARGADO_1);

	struct pokemon *p2 = tp1_buscar_pokemon(file, POKEMON_CASE_SENSITIVE);
	pa2m_afirmar(p2 != NULL, "El pokemon buscado es encontrado sin importar el case sensitive");

	tp1_destruir(file);
}

void buscar_pokemon_inexistente()
{
    tp1_t *file = tp1_leer_archivo(ARCHIVO_CARGADO);

	pa2m_afirmar(tp1_buscar_pokemon(file, POKEMON_INEXISTENTE) == NULL,
		    "Buscar un pokemon inexistente devuelve NULL");

	tp1_destruir(file);
}

//Testeos de buscar por orden
void buscar_orden_null()
{
    pa2m_afirmar(tp1_buscar_orden(NULL, 0) == NULL,
		    "Buscar por orden en un tp1 NULL devuelve NULL");
}

void buscar_orden_f_l()
{
    tp1_t *file = tp1_leer_archivo(ARCHIVO_CARGADO);
	size_t tope = tp1_cantidad(file);

	struct pokemon *primero = tp1_buscar_orden(file, 0);
	pa2m_afirmar(primero != NULL, "Buscar la priemra posición (0) devuelve un pokemon válido");

	struct pokemon *ultimo = tp1_buscar_orden(file, tope - 1);
	pa2m_afirmar(ultimo != NULL, "Buscar la última posición (tope - 1) válida devuelve un pokemon");

	tp1_destruir(file);
}

void buscar_orden_invalido()
{
	tp1_t *file = tp1_leer_archivo(ARCHIVO_CARGADO);
	size_t tope = tp1_cantidad(file);

	pa2m_afirmar(tp1_buscar_orden(file, tope) == NULL,
		    "Buscar un índice fuera de rango (tope) devuelve NULL");
	pa2m_afirmar(tp1_buscar_orden(file, tope + FUERA_DE_RANGO) == NULL,
		    "Buscar un índice muy fuera de rango devuelve NULL");

	tp1_destruir(file);
}

void iterar_null()
{

	tp1_t *file = tp1_leer_archivo(ARCHIVO_CARGADO);
	tp1_t *file_vacio = tp1_leer_archivo(ARCHIVO_VACIO);

	pa2m_afirmar(tp1_iterar(NULL, iterar_siempre, NULL) == 0,
		    "Iterar un tp1 NULL devuelve 0 invocaciones");
	pa2m_afirmar(tp1_iterar(file, NULL, NULL) == 0,
		    "Iterar con una función NULL devuelve 0 invocaciones");
	pa2m_afirmar(tp1_iterar(file_vacio, iterar_siempre, NULL) == 0,
		    "Iterar un tp1 vacío devuelve 0 invocaciones");

	tp1_destruir(file);
	tp1_destruir(file_vacio);
}

void iterar_completo()
{

	tp1_t *file = tp1_leer_archivo(ARCHIVO_CARGADO);
	size_t cant_total = tp1_cantidad(file);

	int nada = 0;

	size_t invocaciones = tp1_iterar(file, iterar_siempre, &nada);
	pa2m_afirmar(invocaciones == cant_total,
		    "Iterar una función que siempre devuelve true invoca la función %d vece/s. Veces iteradas: %ld", cant_total, invocaciones);

}

//Testeos de funcion iterar
void iterar_nombre_datos()
{

	tp1_t *file = tp1_leer_archivo(ARCHIVO_CARGADO);
	tp1_t *file_vacio = tp1_leer_archivo(ARCHIVO_VACIO);
	size_t cant_total = tp1_cantidad(file);

	char *pokemon_buscado = "Magikarp";

	size_t it_busqueda = tp1_iterar(file, buscando_un_pokemon, pokemon_buscado);
	pa2m_afirmar(it_busqueda > 0,
		    "Buscar un pokemon existente se detiene al encontrarlo y devuelve las iteraciones realizadas, pokemon buscado: %s", pokemon_buscado);

	
	int velocidad_buscada = 50;
	size_t contador_velocidad = tp1_iterar(file, buscar_misma_velocidad, &velocidad_buscada);
	pa2m_afirmar(contador_velocidad > 0,
		    "Buscar por velocidad se detiene correctamente en la coincidencia. Velocidad buscada: %d, cantidad de pokemones encontrados: %ld", velocidad_buscada, contador_velocidad);

	tp1_destruir(file);
	tp1_destruir(file_vacio);
}


int main () 
{
    pa2m_nuevo_grupo("Pruebas de lectura de archivos | tp1_lecura_archivos()");
    lectura_error();
    lectura_correcta();
    lectura_vacio_e_inexistente();

    pa2m_nuevo_grupo("Pruebas de cantidad de pokemones | tp1_cantidad()");
    cantidades();

    pa2m_nuevo_grupo("Pruebas para escribir un archivo nuevo | tp1_escribir_archivo()");
    escribir_archivo_null();
    escribir_archivo_exito();

	pa2m_nuevo_grupo("Pruebas para combinar dos tp1 | tp1_combinar()");
    combinar_exito();
    combinaciones_con_null();
    combinar_tp_vacio();

	pa2m_nuevo_grupo("Pruebas para buscar cierto pokemon | tp1_buscar_pokemon()");
    buscar_pokemon_especifico();
    buscar_pokemon_case_sensitive();
    buscar_pokemon_inexistente();
    buscar_pokemon_null();

	pa2m_nuevo_grupo("Pruebas para buscar ordenadamente cierto pokemon | tp1_buscar_orden()");
    buscar_orden_f_l();
    buscar_orden_invalido();
    buscar_orden_null();

	pa2m_nuevo_grupo("Pruebas de iterar sobre los pokemones | tp1_iterar()");
	iterar_null();
	iterar_completo();
	iterar_nombre_datos();

    return EXITO;
}