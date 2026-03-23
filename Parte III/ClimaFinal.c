
#include <stdio.h> //incluir la biblioteca estándar de entrada/salida para usar printf
#include <stdlib.h> //incluir la biblioteca estándar para usar malloc y rand
#include <pthread.h> //incluir la biblioteca de hilos para usar pthread_create y pthread_join

#define NUM_LECTURAS 10000000 //definir el número total de lecturas de temperatura que se van a analizar
#define NUM_HILOS 4 //definir el número de hilos que se van a utilizar para analizar las temperaturas

float *temperaturas; //declarar un puntero a float para almacenar las temperaturas que se van a analizar

typedef struct { //definir una estructura para pasar los datos necesarios a cada hilo
    int id_hilo; //identificador del hilo
    int indice_inicio; //índice de inicio del rango de temperaturas que el hilo va a analizar
    int indice_fin; //índice de fin del rango de temperaturas que el hilo va a analizar
} DatosRango; //definir la función que cada hilo va a ejecutar para analizar su rango de temperaturas 

void* analizar_temperaturas(void* arg) { //definir la función que cada hilo va a ejecutar para analizar su rango de temperaturas 
    DatosRango* datos = (DatosRango*)arg; //convertir el puntero genérico arg a un puntero a DatosRango para acceder a los datos necesarios para el análisis

    int contador_local = 0; //contador propio del hilo
    
    for (int i = datos->indice_inicio; i < datos->indice_fin; i++) { //recorrer el rango de temperaturas asignado al hilo
        if (temperaturas[i] > 35.0f) { //si la temperatura es mayor a 35 grados Celsius, se considera una alerta de calor extremo
            contador_local++; //incrementar el contador de alertas de calor extremo
        }
    }
    //Reservar memoria para devolver el resultado
    int* resultado = malloc(sizeof(int));
    *resultado = contador_local;

    //Liberar la memoria de los datos del hilo
    free(datos);
    return resultado;
} 


int main(void) { //definir la función principal del programa

    temperaturas = malloc(NUM_LECTURAS * sizeof(float)); //asignar memoria para almacenar las temperaturas que se van a analizar
    
    //Inicializar temperaturas
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
    int total_alertas = 0;
     
    for (int i = 0; i < NUM_HILOS; i++) { //Noten que el join puede esperar todos los hilos en un for con el ID de cada uno.
        void* retorno;

        pthread_join(hilos[i], &retorno); //esperar a que cada hilo termine su ejecución antes de continuar con el programa principal, pasando el identificador del hilo y un puntero para almacenar el valor de retorno del hilo (en este caso, NULL ya que la función analizar_temperaturas no retorna ningún valor)
        
        int* valor = (int*)retorno;
        total_alertas += *valor;

        free(valor);//liberar resultado del hilo

    }

    printf("Análisis completo. Total de alertas de calor extremo: %d \n", total_alertas); //imprimir el resultado del análisis, mostrando el número total de alertas de calor extremo encontradas
    free(temperaturas); //liberar la memoria asignada para almacenar las temperaturas que se han analizado
    return 0; //retornar 0 para indicar que el programa ha terminado su ejecución de manera exitosa
} 


