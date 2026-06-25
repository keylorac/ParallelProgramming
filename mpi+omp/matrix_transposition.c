#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>

//transposicion de matrices por bloques (cache blocking)
double **matriz_a;
double **matriz_b; // porque usamos malloc para reservar memoria dinamicamente para las matrices, ya que el tamano de la matriz es muy grande y no se puede reservar en la pila (stack) del programa, ademas de que malloc nos permite reservar memoria en tiempo de  

void transponer_matriz(int n, int m, int block_size) {
    #pragma omp parallel for collapse(2) //usamos collapse(2) para parelilizar los bucles anidados (que son dos)
    for (int i = 0; i < n; i += block_size) {
        for (int j = 0; j < m; j += block_size) {
            // transponer el bloque de tamano block_size x block_size
            for (int k = i; k < i + block_size && k < n; k++) {
                for (int l = j; l < j + block_size && l < m; l++){
                    matriz_b[l][k] = matriz_a[k][l];
                }
            }
        }
    }
}
int main (int argc, char *argv[]){
    //inicializamos mpi
    int rank, size;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Definimos el tamano de la matriz y el tamano del bloque
    int n, m, block_size; 
    n = m = 16384;
    block_size = 64;
    
    // Reservamos memoria para las matrices
    matriz_a = (double **)malloc(n * sizeof(double *));
    matriz_b = (double **)malloc(m * sizeof(double *));

    for (int i = 0; i < n; i++) {
        matriz_a[i] = (double *)malloc(m * sizeof(double));
    }
    for (int i = 0; i < m; i++){
        matriz_b[i] = (double *)malloc(n * sizeof(double));
    }
    if (rank == 0) {
        printf("Transponiendo una matriz de %d x %d con bloques de tamano %d\n", n, m, block_size);
    }

    // inicializamos la matriz_a con valores aleatorios
    for (int i = 0; i < n; i++){
        for (int j = 0; j < m; j++){
            matriz_a[i][j] = (double)rand() / RAND_MAX; //usamos rand() para generar valores aleatorios entre 0 y 1 y RAND_MAX para normalizar el valor 

        }
    }
    // medicion del tiempo
    double start_time = MPI_Wtime(); //usamos MPI_Wtime()
    // transponemos la matriz
    transponer_matriz(n, m, block_size);
    double end_time = MPI_Wtime();

    if (rank == 0){
        printf("Tiempo de ejecucion: %f segundos \n", end_time - start_time);
    }
    //liberamos la memoria de las matrices
    /*for (int i = 0; i < n; i++){
        free(matriz_a[i]);
    }
    */
    for (int i = 0; i < m; i++) free(matriz_a[i]);

    /*for (int i = 0; i < m; i++){
        free(matriz_b[i]);
    }
    */
    for (int i = 0; i < n; i++) free(matriz_b[i]);
    
    free(matriz_a);
    free(matriz_b);

    //finalizamos mpi
    MPI_Finalize();

    //retornamos 0 para inidicar que el programa se ejecuto correctamente
    return 0;
}