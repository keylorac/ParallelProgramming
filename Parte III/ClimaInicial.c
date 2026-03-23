
 /* IMPORTANTE PARA LA COMPILACIÓN: gcc -g -Wall clima.c -lpthread -o clima 
 Necesitan agregar -g para que Valgrind de información necesaria para el laboratorio */

#include <stdio.h> //incluir la biblioteca estándar de entrada/salida para usar printf
#include <stdlib.h> //incluir la biblioteca estándar para usar malloc y rand
#include <pthread.h> //incluir la biblioteca de hilos para usar pthread_create y pthread_join

#define NUM_LECTURAS 10000000 //definir el número total de lecturas de temperatura que se van a analizar
#define NUM_HILOS 4 //definir el número de hilos que se van a utilizar para analizar las temperaturas

float *temperaturas; //declarar un puntero a float para almacenar las temperaturas que se van a analizar
int alertas_calor = 0; //declarar una variable global para contar el número de alertas de calor extremo encontradas
typedef struct { //definir una estructura para pasar los datos necesarios a cada hilo
    int id_hilo; //identificador del hilo
    int indice_inicio; //índice de inicio del rango de temperaturas que el hilo va a analizar
    int indice_fin; //índice de fin del rango de temperaturas que el hilo va a analizar
} DatosRango; //definir la función que cada hilo va a ejecutar para analizar su rango de temperaturas asignado

void* analizar_temperaturas(void* arg) { //definir la función que cada hilo va a ejecutar para analizar su rango de temperaturas asignado
    DatosRango* datos = (DatosRango*)arg; //convertir el puntero genérico arg a un puntero a DatosRango para acceder a los datos necesarios para el análisis
    
    for (int i = datos->indice_inicio; i < datos->indice_fin; i++) { //recorrer el rango de temperaturas asignado al hilo
        if (temperaturas[i] > 35.0f) { //si la temperatura es mayor a 35 grados Celsius, se considera una alerta de calor extremo
            alertas_calor++; //incrementar el contador de alertas de calor extremo
        }//Nota: Este incremento no es seguro en un ambiente concurrente (condicion de carrera), pero lo dejamos así para simplificar el ejemplo. En un ambiente real, se debería usar un mutex para proteger esta variable compartida.
    }
    return NULL; //retornar NULL para indicar que el hilo ha terminado su ejecución
} 

int main(void) { //definir la función principal del programa
    temperaturas = malloc(NUM_LECTURAS * sizeof(float)); //asignar memoria para almacenar las temperaturas que se van a analizar
    for (int i = 0; i < NUM_LECTURAS; i++) { //llenar el arreglo de temperaturas con valores aleatorios entre 30 y 40 grados Celsius
        temperaturas[i] = 30.0f + (float)(rand() % 10); //rand() % 10 genera un número aleatorio entre 0 y 9, que se convierte a float y se suma a 30 para obtener un valor entre 30.0f y 39.0f
    } 

    pthread_t hilos[NUM_HILOS]; //declarar un arreglo de hilos para almacenar los identificadores de los hilos que se van a crear

    printf("Iniciando análisis \n"); //imprimir un mensaje indicando que se va a iniciar el análisis de las temperaturas

    for (int i = 0; i < NUM_HILOS; i++) { //crear los hilos para analizar las temperaturas en paralelo
        DatosRango* datos = malloc(sizeof(DatosRango)); //asignar memoria para almacenar los datos necesarios para el análisis de cada hilo
        datos->id_hilo = i; //asignar el identificador del hilo a los datos del hilo 
        datos->indice_inicio = i * (NUM_LECTURAS / NUM_HILOS); //calcular el índice de inicio del rango de temperaturas que el hilo va a analizar, dividiendo el número total de lecturas entre el número de hilos y multiplicando por el identificador del hilo
        datos->indice_fin = (i + 1) * (NUM_LECTURAS / NUM_HILOS); //calcular el índice de fin del rango de temperaturas que el hilo va a analizar, dividiendo el número total de lecturas entre el número de hilos y multiplicando por el identificador del hilo más uno

        pthread_create(&hilos[i], NULL, analizar_temperaturas, datos); //crear un hilo para ejecutar la función analizar_temperaturas, pasando los datos necesarios para el análisis del hilo como argumento
    }

    for (int i = 0; i < NUM_HILOS; i++) { //Noten que el join puede esperar todos los hilos en un for con el ID de cada uno.
        pthread_join(hilos[i], NULL); //esperar a que cada hilo termine su ejecución antes de continuar con el programa principal, pasando el identificador del hilo y un puntero para almacenar el valor de retorno del hilo (en este caso, NULL ya que la función analizar_temperaturas no retorna ningún valor)
    }

    printf("Análisis completo. Total de alertas de calor extremo: %d \n", alertas_calor); //imprimir el resultado del análisis, mostrando el número total de alertas de calor extremo encontradas
    free(temperaturas); //liberar la memoria asignada para almacenar las temperaturas que se han analizado
    return 0; //retornar 0 para indicar que el programa ha terminado su ejecución de manera exitosa
} //Nota: Este programa no es seguro en un ambiente concurrente debido al acceso no sincronizado a la variable global alertas_calor. En un ambiente real, se debería usar un mutex para proteger esta variable compartida y evitar condiciones de carrera.

/*NOTAS DE LOS PROBLEMAS ENCONTRADOS EN EL CODIGO*/
/*  1. AL correr el programa, se pueden obtener resultados diferentes en cada ejecución debido a la condición de carrera en la variable global alertas_calor. Esto se debe a que varios hilos pueden estar incrementando esta variable al mismo tiempo sin ninguna sincronización, lo que puede llevar a resultados incorrectos o inconsistentes. 
    2. Con el uso de Valgrind memcheck, se pueden detectar problemas de memoria en el programa, como accesos a memoria no inicializada, fugas de memoria o accesos a memoria fuera de los límites del arreglo. En este programa, se podrían detectar problemas como el acceso a la variable global alertas_calor sin protección adecuada, lo que podría llevar a resultados incorrectos o inconsistentes. Además, se podrían detectar problemas relacionados con la asignación y liberación de memoria para el arreglo de temperaturas y los datos de cada hilo, asegurando que no haya fugas de memoria o accesos a memoria no válida.
    3. Con el uso de Valgrind helgrind, se pueden detectar problemas de concurrencia en el programa, como condiciones de carrera o accesos a variables compartidas sin sincronización adecuada. En este programa, se podrían detectar la condición de carrera en la variable global alertas_calor, lo que indicaría que varios hilos están accediendo a esta variable al mismo tiempo sin protección adecuada. Esto ayudaría a identificar y solucionar el problema de concurrencia en el programa, asegurando que los resultados sean correctos y consistentes en cada ejecución.
    4. Error grave: tenemos resultados diferentes en cada ejecución debido a la condición de carrera en la variable global alertas_calor, lo que puede llevar a resultados incorrectos o inconsistentes. Esto se debe a que varios hilos pueden estar incrementando esta variable al mismo tiempo sin ninguna sincronización, lo que puede causar que el valor final de alertas_calor sea menor o mayor de lo esperado. 
*/