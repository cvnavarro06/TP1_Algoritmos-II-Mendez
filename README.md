# TP1 - Pokédex
---

- **Nombre** = Constantino Valentín Navarro
- **Padrón** = 114821
- **E-mail** = cvnavarro@fi.uba.ar
- **User de Github** = cvnavarro06

---

## Instrucciones para el uso correcto del programa

### Compilación

```
make compile
```
---
### Correr programa
```
./<ejecutable> <archivo_csv> <comandos>
```
---
### Comandos para ejecutar
```
make commands
```


---

### Correr pruebas
#### Primero compilo las mismas
```
make compile_test NOMBRE_TEST=[Opcional]
```
#### Correr ruebas
```
make test
```

---
### Correr valgrind
>**Aclaración**: Para usar _valgrind_, previamente se debe de compilar.
```
./valgrind --leak-check=full --track-origins=yes --show-reachable=yes --error-exitcode=2 --show-leak-kinds=all --trace-children=yes -s ./<ejecutable> <archivo_csv> <comandos>
```
---

### Para más información
```
make manual
```


## Funcionamiento del programa
El programa implementa una pokédex que brinda distintas funcionalidades, para el funcionamiento del programa el mismo requiere de un archivo CSV el deberá de contener la siguiente estructura: `nombre,velocidad,peso,rareza`.

Algunas de sus funcionalidades son por ejemplo combinar pokédexs haciendo una unión, se pude iterar `n` veces una pokédex ejecutando una función a elección que impacta sobre el pokemon, también se puede escribir un archivo nuevo con una pokédex ya cargada y por último se puede buscar un pokemon especificado por `nombre` y también por posición en la estructura de datos.

### Estructura de datos
La estructura elegida para almacenar los datos del archivo CSV es un vector dinámico el cuál es definido con el struct `tp1_t`, y la forma de estructurarse es la siguiente:
```
tp1{
    struct pokemon *pokemones; --> Puntero a un struct pokémon.
    size_t cantidad;           --> Cantidad de pokemones cargados en tp1.
    size_t espacio_m;          --> Espacio ocupando en memoria (HEAP)
} tp1_t;
```
#### ¿Por qué se decidio esa estructura?
La elección de este tipo de estructura nacio como unicamente un vector dinámico el cual contiene structs, de alguna manera debía tener la capacidad de saber contabilizar la cantidad de pokemones, a raíz de ese inconveniente nace `size_t cantidad`. <br>
Y como último está `size_t espacio_m`, el cual lo implemente en primera instancia para conocer cuanta espacio estaría ocupando en el heap, como beneficio se obtubo que a la hora de hacer la función `tp1_combinar()` puedo sumar el espacio de ambos `tp1` para poder tener un espacio más dedicado al nuevo `tp1`.
#### Diagrama de memoria de la estructura


## Funciones del `tp1.c`

### `tp1_leer_archivo()`
---
Abre y lee un archivo CSV con datos de pokemones. Del mismo, se extraerá los datos (nombre, velocidad, peso y rareza) línea por línea, para dicha lectura se hace llamado a la función `leer_linea()`y con el resultado de la misma función, se almacenan, los pokemones, en la estructura principal `tp1_t` y al finalizar la carga, los ordena a todos alfabéticamente por nombre y elimina los pokemones duplicados.

#### Diagrama de la función

#### Diagrama de memoria

#### Complejidad
**O(N²)**: El programa empieza con la lectura del archivo y el guardado inicial, los cuales tomará una complejidad `O(N)`(Producto del while), al finalizar la carga se llama a la función ordenar_pokemones, la cuál utiliza un algoritmo de ordenamiento por inserción, cuya complejidad es `O(N²)`en el peor de los casos y como último, se ejecuta eliminar_duplicados, que también contiene un bucle anidado desplazando elementos, operando en `O(N²)`.

### `tp1_cantidad()`
---
Esta función devolverá la cantidad total de pokemones válidos que están almacenados actualmente dentro de la estructura `tp1_t`. En caso de que se reciba un puntero nulo, la función misma devolverá 0.
#### Diagrama de la función


#### Complejidad
**O(1)**: La función solo devuelve un campo de la estructura (`tp1->cantidad`), lo cual es una operación de tiempo constante.

### `tp1_combinar()`
---
Se toman dos estructuras `tp1_t` (`tp1_a` y `tp1_b`) y crea una nueva estructura resultante de combinar los pokemones de ambas. Realiza la mezcla manteniendo el orden alfabético. En caso de encontrar pokemones con el mismo nombre, le da prioridad y guarda únicamente al pokemon perteneciente al primer TP1 (tp1_a).
En caso de que queden pokemones restantes de alguno de las 2 estructuras, se terminarán de cargar al `tp1` a devolver.

#### Diagrama de la función

#### Complejidad
**O(n + m)**: n y m serán la cantidad de pokemones en cada `TP1` respectivamente. Recordemos que la función recorre de manera simultánea los elementos de ambos vectores ordenados, evaluando y copiando elementos una sola vez hasta recorrer ambos `tp1`.

### `tp1_escribir_archivo()`
---
Esta función tomará una estructura `tp1_t` cargada y guarda toda la información de sus pokemones en un nuevo archivo de texto con el formato `(nombre,velocidad,peso,rareza)`. Si el archivo de destino ya existe se sobreescribirá.

#### Diagrama de la función

#### Complejidad
**O(N)**: La función itera linealmente utilizando un bucle `for` desde el inicio hasta tp1->cantidad para realizar las operaciones necesarias.

### `tp1_buscar_pokemon()`
---
Esta función buscará un pokemon específico por su nombre dentro de la estructura `tp1_t`, sin importar si está escrito en mayúsculas o minúsculas (no es _case-sensitive_), en caso de encontrarlo, se devolverá un puntero a dicho pokemon; si no, devuelve **NULL**

#### Diagrama de la función

#### Complejidad
**O(N)**: La función busca el pokemon iterando linealmente por todo el vector utilizando un bucle `for` y evaluando cada `nombre` hasta toparse con el enviado por parámetro. En el peor de los casos, debe recorrer todos los elementos para determinar que no existe o encontrarlo al final del vector.

### `tp1_buscar_orden()`
---
Esta función devolverá un puntero al pokemon que se encuentra en la posición **'n'** solicitada del `struct pokemon`. Si el índice pedido es mayor o igual a la cantidad de pokemones guardados, devuelve **NULL**.

#### Diagrama de la función

#### Complejidad
**O(1)**: Al estar los elementos guardados de forma orndenada en un vector, la función simplemente realiza un acceso directo a la posición solicitada (`&tp1->pokemones[n]`) validando previamente que no se excedan los límites.

### `tp1_iterar()`
---
Esta función iterará el `struct pokemones` de inicio a fin, enviándole cada pokemon por parámetro a una función externa `f` la cuál mandará, por parámetro, por el usuario. La iteración continúa siempre y cuando la función `f` devuelva `true`, y se detiene inmediatamente si esta misma devuelve `false`. <br>
El valor que devolverá esta función será la cantidad total de veces que la función `f` fue invocada.

#### Diagrama de la función

#### Complejidad
**O(N)**: La iteración principal recorre el vector de pokemones una vez en el peor de los casos **O(N)** a causa del `for`.  

### `tp1_destruir()`
---
Liberará de manera limpia y segura toda la memoria dinámica que fue solicitada durante la "vida" útil del `TP1`, también se encarga de limpiar los strings de los nombres, el vector principal (`struct pokemon`) y la estructura (`tp1`).

#### Diagrama de la función

#### Complejidad
**O(N)**: Para liberar toda la memoria de forma rigurosa, debe ejecutar un bucle `for` que itere exactamente `tp1->cantidad` veces para hacer `free` de cada uno de los "strings" de los nombres antes de liberar el resto del vector.