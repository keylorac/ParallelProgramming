# Lab 1 Hilos e Indeterminismo

En el presente Readme se desarrollará el uso de Valgrind para la detección de errorres en el código, y de haber alguno, se especificará.

Al ejecutar el programa, no aparecen fugas de memoria ni accesos inválidos. La memoria ckasificada como "still reachable" corresponde a una flag de las bibbliotecas internas de MPI, pero no es un problema en la implementacion del programa.

En el programa no se utiliza malloc(), no hay arrays, ni hay punteros manipulados manualmente, por lo que Valgrind no reporta errores. Además, las funciones MPI_Init() y MPI_Finalize() están utilizadas adecuadamente.

Sin embargo, Valgrind no marca problemas de concurrencia. Esa parte habría que analizarla con Helgrind.