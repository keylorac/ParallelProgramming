//Importar bibliotecas
#include <stdio.h> // Entrada y salida de datos
#include <stdlib.h> // Manejo de memoria dinamica
#include <pthread.h> //Para la creacion de hilos

// Estructura de datos para agrupar los valores para pasarlos como unicos al hilo
typedef struct {
int valor_a; //valor a
int valor_b; //valor b
} DatosEntrada;

//Funcion para calcular la suma de los datos ingresados
void* calcular_suma(void* arg) { //Funcion solo acepta void* como parametro
DatosEntrada* datos = (DatosEntrada*)arg; // Puntero a los valores a y b
int resultado_local = datos->valor_a + datos->valor_b; // Suma de los valores
int* valor_retorno = malloc(sizeof(int)); // Asigna el tamanio de memoria en valor_retorno
*valor_retorno = resultado_local; //Asigna el resultado
return (void*)valor_retorno; //Retorna el valor
}
int main(void) {
printf("Iniciando la creacion del hilo... \n "); // Printea la creacion del hilo

pthread_t mi_hilo; //Creacion de variable tipo pthread
DatosEntrada argumentos = {15, 25}; //Asigna los valores para la suma
int* resultado_final; // Variable para guardar lo que se va a retornar
if (pthread_create(&mi_hilo, NULL, calcular_suma, &argumentos) != 0) {
    printf("Los valores dados son: %d y %d \n", argumentos.valor_a, argumentos.valor_b); //Printea los valores

    printf("Terminando creacion del hilo... \n");
    return 1;
} // Creacion del hilo con sus parametros

printf("Hilo creado, esperando finalizacion... \n");
pthread_join(mi_hilo, (void**)&resultado_final); // Espera que el hilo termine para recuperar el valor
printf("Hilo terminado, resultado obtenido: %d ",  *resultado_final); //Mostrar resultado final
free(resultado_final); //libera memoria del malloc

return 0;
}