# Informe de errores en el archivo - clima.c

## 1. Comportamiento observado

Al ejecutar el programa varias veces, se obtienen resultados diferentes en cada ejecucion. Por ejemplo: 

```c
Iniciando analisis
Iniciando analisis
Analisis completo. Total de alertas de calor extremo: 2957191
Análisis completo. Total de alertas de calor extremo: 2492677
```

Esto indica que el programa no es determinista, lo cual es una senal clara de un problema de concurrencia.

---

## 2. Problema principal: condicion de carrera

EL problema mas importante del programa es una **condicion de carrera** en la variable global:

```c
int alertas_calor = 0;

Esta variable es compartida entre multiples hilos, y se incrementa con:

```
alertas_calor++;

Varios hilos ejecutan esta instruccion al mismo tiempo sin ningun mecanismo de sincronizacion, lo cual produce accesos concurrentes que generan resultados incorrectos.

## 3. Evidencia con Helgrind

Se ejecuto el comando:

```c
valgrind --tool=helgrind ./clima
````


Y obtuvimos el siguiente resultado: 

```c
keylor@keylor-ASUSLaptop-Q540VJ:~/~Documentos/CI0117/Lab01$ valgrind --tool=helgrind ./clima
==14724== Helgrind, a thread error detector
==14724== Copyright (C) 2007-2017, and GNU GPL'd, by OpenWorks LLP et al.
==14724== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==14724== Command: ./clima
==14724== 
Iniciando análisis 
==14724== ---Thread-Announcement------------------------------------------
==14724== 
==14724== Thread #3 was created
==14724==    at 0x49A4A53: clone (clone.S:76)
==14724==    by 0x49A4BD2: __clone_internal_fallback (clone-internal.c:64)
==14724==    by 0x49A4BD2: __clone_internal (clone-internal.c:109)
==14724==    by 0x491754F: create_thread (pthread_create.c:297)
==14724==    by 0x49181A4: pthread_create@@GLIBC_2.34 (pthread_create.c:836)
==14724==    by 0x4854975: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==14724==    by 0x1093BA: main (clima.c:48)
==14724== 
==14724== ----------------------------------------------------------------
==14724== 
==14724== Possible data race during read of size 4 at 0x10C020 by thread #3
==14724== Locks held: none
==14724==    at 0x10926E: analizar_temperaturas (clima.c:25)
==14724==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==14724==    by 0x4917AA3: start_thread (pthread_create.c:447)
==14724==    by 0x49A4A63: clone (clone.S:100)
==14724==  Address 0x10c020 is 0 bytes inside data symbol "alertas_calor"
==14724== 
==14724== ----------------------------------------------------------------
==14724== 
==14724== Possible data race during write of size 4 at 0x10C020 by thread #3
==14724== Locks held: none
==14724==    at 0x109277: analizar_temperaturas (clima.c:25)
==14724==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==14724==    by 0x4917AA3: start_thread (pthread_create.c:447)
==14724==    by 0x49A4A63: clone (clone.S:100)
==14724==  Address 0x10c020 is 0 bytes inside data symbol "alertas_calor"
==14724== 
Análisis completo. Total de alertas de calor extremo: 4001277 
==14724== 
==14724== Use --history-level=approx or =none to gain increased speed, at
==14724== the cost of reduced accuracy of conflicting-access information
==14724== For lists of detected and suppressed errors, rerun with: -s
==14724== ERROR SUMMARY: 6872 errors from 2 contexts (suppressed: 6 from 2)
```

Eso quiere decir que multiples hilos estan leyendo y escribiendo la variable `alertas_calor` al mismo tiempo sin sincronizacion.

## 4. Ubicacion del problema

EL problema ocurre especificamente en la funcion 

```c
void* analizar_temperaturas(void* arg)
```

En la linea

```c
alertas_calor++;
```

Se produce la condicion de carrera.

## 4. Problemas de memoria (Memcheck)

Al utilizar Memcheck para analizar posibles errores de memoria:

```c
valgrind --leak-check=full ./clima
```

El programa presenta la siguiente salida:

```c
keylor@keylor-ASUSLaptop-Q540VJ:~/~Documentos/CI0117/Lab01$ valgrind --leak-check=full ./clima
==17275== Memcheck, a memory error detector
==17275== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==17275== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==17275== Command: ./clima
==17275== 
Iniciando análisis 
Análisis completo. Total de alertas de calor extremo: 4001277 
==17275== 
==17275== HEAP SUMMARY:
==17275==     in use at exit: 48 bytes in 4 blocks
==17275==   total heap usage: 10 allocs, 6 frees, 40,002,160 bytes allocated
==17275== 
==17275== 48 bytes in 4 blocks are definitely lost in loss record 1 of 1
==17275==    at 0x4846828: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==17275==    by 0x10934D: main (clima.c:42)
==17275== 
==17275== LEAK SUMMARY:
==17275==    definitely lost: 48 bytes in 4 blocks
==17275==    indirectly lost: 0 bytes in 0 blocks
==17275==      possibly lost: 0 bytes in 0 blocks
==17275==    still reachable: 0 bytes in 0 blocks
==17275==         suppressed: 0 bytes in 0 blocks
==17275== 
==17275== For lists of detected and suppressed errors, rerun with: -s
==17275== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
```
Se detecta una fuga de memoria de **48 bytes en 4 bloques**, lo que indica que hay memoria reservada que no fue liberada correctamente.

Cada bloque corresponde a una estructura de `DatosRango` creada con:

```c
DatosRango* datos = malloc (sizeof(DatosRango));
```

Dado que nuestro programa ejecuta:

```c
#define NUM_HILOS 4
```

Se crean 4 estructuras dinamicas. Estas estructuras no son liberadas, por eso provoca una fuga de memoria en cada hilo.

En resumen tenemos 2 problemas grandes:

1. Fuga de memoria 

```c
DatosRango * datos = ...
sin free()
```

Cuya solucion es agregar:

```c
free(datos);
```

2. Race condition

```c
alertas_calor+++;
```
Acceso concurrente sin sincronizacion.

Para solucionar la condicion de carrera, se elimina la variable global compartida `alertas_calor` .

En su lugar, cada hilo utiliza un contador local para almancenar sus resultados. Al finalizar, cada hilo retorna su resultado al hilo principal mediante `pthread_join`, donde se suman todos los valores.

De esta forma, se evita el acceso concurrente a memoria compartida, eliminando la condicion de carrera.

