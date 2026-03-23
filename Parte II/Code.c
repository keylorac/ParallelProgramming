#define _XOPEN_SOURCE 500 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h> 

// Específicamente ¿En que segmento se almacenan estas variables?
long long vida_Escudo = 50000; // ¿Qué tamaño tiene esta variable?
const long long ESCUDO_MAX = 100000;
volatile bool mision_Activa = true; 

pthread_mutex_t io_mutex = PTHREAD_MUTEX_INITIALIZER; 

//¿Qué hace cada uno de las siguientes 2 funciones?

void* EscuadronDefensa(void* arg) { //Estas funciones de hilos deben retornar un puntero genérico, para eso es el void*.
    long long iteraciones = *(long long*)arg; //Se trata el puntero genérico arg como si fuera un puntero a un número de 64 bits (el * al inicio es de desreferenciación)
    for (long long i = 0; i < iteraciones; ++i) {
        vida_Escudo++; 
    }
    return NULL;
}

void* EscuadronAtaque(void* arg) { 
    long long iteraciones = *(long long*)arg; //
    for (long long i = 0; i < iteraciones; ++i) {
        vida_Escudo--; 
    }
    return NULL;
}

void* Radar(void* arg) {
    while (mision_Activa) {
        pthread_mutex_lock(&io_mutex);
        printf("\r Integridad del escudo: %lld / %lld   ", vida_Escudo, ESCUDO_MAX); // /r sobreescribe la línea actual, por si lo quieren utilizar en otros contextos.
        fflush(stdout); 
        pthread_mutex_unlock(&io_mutex);
        
        usleep(50000); //refrescar cada 50 ms
    }
    return NULL;
}

#define NUM_UNIDADES 4 

/*Si tenemo un escudo inicial de 50 000, tenemos 4 defensores y 4 atacantes que van a realizar 100 000 000 de iteraciones ¿Cuál sería el valor del escudo final en un ambiente secuencial?*/

int main(void) {
    long long iteraciones_equipos = 100000000; 
    pthread_t defensores[NUM_UNIDADES]; //Veanlo como un número de ID para cada defensor y atacante
    pthread_t atacantes[NUM_UNIDADES];
    pthread_t radar_hilo; //Un hilo para el radar
    printf("Iniciando operaciones sobre el escudo: \n");
    
    pthread_create(&radar_hilo, NULL, Radar, NULL); //Aquí nace la concurrencia. Luego veremos más sobre esto en clase

    for (int i = 0; i < NUM_UNIDADES; ++i) { //Se crean un forpara crear las unidades juntas
        pthread_create(&defensores[i], NULL, EscuadronDefensa, &iteraciones_equipos);
        pthread_create(&atacantes[i], NULL, EscuadronAtaque, &iteraciones_equipos);
    }

    for (int i = 0; i < NUM_UNIDADES; ++i) { //Esto bloquea el avance hasta que las unidades hayan vuelto, pueden verlo como el punto de reunión
        pthread_join(defensores[i], NULL);
        pthread_join(atacantes[i], NULL);
    }

    mision_Activa = false;
    pthread_join(radar_hilo, NULL);
    printf("\n \nReporte final del escudo\n");
    printf("Integridad proyectada: 50000\n");
    printf("Integridad real: %lld\n", vida_Escudo);
    return 0;
}