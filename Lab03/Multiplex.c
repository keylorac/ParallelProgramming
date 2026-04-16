#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define TOTAL_VUELOS 15
#define PISTAS_DISPONIBLES 3

/*Lista a realizar para el laboratorio:
    1.1 Declare sus primitivas de sincronización globales.
    1.2 Necesitará al menos un semáforo de conteo y un mutex. (LISTO)
    1.3 Inicialice las primitivas de sincronización en el lugar correcto.
    2.1 Proteja la sección de impresión.
    2.2 Recuerde liberar la estructura.
    3.1 Implemente la entrada al Multiplex.
    3.2 Si no hay pistas, el hilo debe bloquearse.
    4.1 Incremente pistas_ocupadas de forma segura.
    4.2 Decremente pistas_ocupadas de forma segura.
    4.3 Utilice solo la estructura necesaria para realizar el manejo de pistas_ocupadas.
    5.1 Señalice la salida del multiplex para darle la pista a un avión en espera.
    6.1 Cree los hilos con rutina_vuelo y el ID correspondiente.
    7.1 Espere los hilos al terminar en el lugar correcto con un join().
    7.2 Recuerde destruir las primitivas de sincronización.
*/

sem_t semaforo_de_pistas;
pthread_mutex_t mutex;


int pistas_ocupadas = 0; // <---- ¡Variable compartida!

//¡Tomen en cuenta que printf no es thread-safe por defecto!
void render_radar_log(int vuelo_id, const char* accion) {
    pthread_mutex_lock(&mutex); 
    printf("Vuelo %02d %-40s | Pistas Ocupadas: %d/%d \n", 
    vuelo_id, accion, pistas_ocupadas, PISTAS_DISPONIBLES);

    pthread_mutex_unlock(&mutex);

}

void* rutina_vuelo(void* arg) {
    int vuelo_id = *(int*)arg;

    render_radar_log(vuelo_id, ": entrando al espacio aereo. Solicitando pista para el aterrizaje.");
    sem_wait(&semaforo_de_pistas);
    pthread_mutex_lock(&mutex); 
    pistas_ocupadas++;
    pthread_mutex_unlock(&mutex);

    render_radar_log(vuelo_id, ">> aterrizando y desembarcando.");


    //Esto simula el tiempo impredecible en pista
    int tiempo_pista = 1 + (rand() % 4); // 1 a 4 segundos
    sleep(tiempo_pista);
    

    
    pthread_mutex_lock(&mutex); 
    pistas_ocupadas--;
    pthread_mutex_unlock(&mutex);
    render_radar_log(vuelo_id, "<< ha despegado. Pista liberada.");
    sem_post(&semaforo_de_pistas);

    return NULL;
}

int main() {
    //Semilla
    srand(time(NULL)); 
    sem_init(&semaforo_de_pistas, 0, PISTAS_DISPONIBLES);
    pthread_mutex_init(&mutex, NULL);

    pthread_t torre_control[TOTAL_VUELOS];
    int ids_vuelos[TOTAL_VUELOS];

    printf("Sistema de control de tráfico aereo ===\n");
    printf("Capacidad del aeropuerto: %d pistas simultáneas. \n\n", PISTAS_DISPONIBLES);

    for(int i = 0; i < TOTAL_VUELOS; i++) {
        ids_vuelos[i] = i + 1;
        pthread_create(&torre_control[i], NULL,rutina_vuelo,&ids_vuelos[i]);
            
        //Esto simula las llegadas de aviones en momentos aleatorios (0 a 0.5s)
        usleep(rand() % 500000); 
    }
    
    for (int i = 0; i < TOTAL_VUELOS; i++)
    {
        pthread_join(torre_control[i], NULL);
    }
    

    printf("\n=== CIERRE DE OPERACIONES. ESPACIO AÉREO DESPEJADO. ===\n");

    sem_destroy(&semaforo_de_pistas);
    pthread_mutex_destroy(&mutex);
    return 0;
}