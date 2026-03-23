# Lab 1 Hilos e Indeterminismo

En el presente Readme se desarrollará el uso de Valgrind para la detección de errorres en el código, y de haber alguno, se especificará.

Al ejecutar el programa con la funcion de Valgrind Memcheck, no aparecen fugas de memoria ni accesos inválidos. La memoria ckasificada como "still reachable" corresponde a una flag de las bibbliotecas internas de MPI, pero no es un problema en la implementacion del programa. Lo que aparece en la terminal es lo siguiente:

==11305== Memcheck, a memory error detector
==11305== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==11305== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==11305== Command: ./code
==11305== 
Iniciando operaciones sobre el escudo: 
 Integridad del escudo: 50000 / 100000   00   
 
Reporte final del escudo
Integridad proyectada: 50000
Integridad real: 50000
==11305== 
==11305== HEAP SUMMARY:
==11305==     in use at exit: 0 bytes in 0 blocks
==11305==   total heap usage: 10 allocs, 10 frees, 3,472 bytes allocated
==11305== 
==11305== All heap blocks were freed -- no leaks are possible
==11305== 
==11305== For lists of detected and suppressed errors, rerun with: -s
==11305== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)

En el programa no se utiliza malloc(), no hay arrays, ni hay punteros manipulados manualmente, por lo que Valgrind no reporta errores. Además, las funciones MPI_Init() y MPI_Finalize() están utilizadas adecuadamente.

Sin embargo, Valgrind Memcheck no marca problemas de concurrencia. Esa parte habría que analizarla con Helgrind.
Ejecutandolo con Helgrind la terminal muestra lo siguiente: 

==11513== Helgrind, a thread error detector
==11513== Copyright (C) 2007-2017, and GNU GPL'd, by OpenWorks LLP et al.
==11513== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==11513== Command: ./code
==11513== 
Iniciando operaciones sobre el escudo: 
 Integridad del escudo: 50000 / 100000   ==11513== ---Thread-Announcement------------------------------------------
==11513== 
==11513== Thread #3 was created
==11513==    at 0x49A6A53: clone (clone.S:76)
==11513==    by 0x49A6BD2: __clone_internal_fallback (clone-internal.c:64)
==11513==    by 0x49A6BD2: __clone_internal (clone-internal.c:109)
==11513==    by 0x491954F: create_thread (pthread_create.c:297)
==11513==    by 0x491A1A4: pthread_create@@GLIBC_2.34 (pthread_create.c:836)
==11513==    by 0x4854975: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x1093DD: main (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513== 
==11513== ---Thread-Announcement------------------------------------------
==11513== 
==11513== Thread #2 was created
==11513==    at 0x49A6A53: clone (clone.S:76)
==11513==    by 0x49A6BD2: __clone_internal_fallback (clone-internal.c:64)
==11513==    by 0x49A6BD2: __clone_internal (clone-internal.c:109)
==11513==    by 0x491954F: create_thread (pthread_create.c:297)
==11513==    by 0x491A1A4: pthread_create@@GLIBC_2.34 (pthread_create.c:836)
==11513==    by 0x4854975: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x1093A7: main (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513== 
==11513== ----------------------------------------------------------------
==11513== 
==11513==  Lock at 0x10C040 was first observed
==11513==    at 0x48512DC: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x1092FB: Radar (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x4919AA3: start_thread (pthread_create.c:447)
==11513==    by 0x49A6A63: clone (clone.S:100)
==11513==  Address 0x10c040 is 0 bytes inside data symbol "io_mutex"
==11513== 
==11513== Possible data race during write of size 8 at 0x10C010 by thread #3
==11513== Locks held: none
==11513==    at 0x109275: EscuadronDefensa (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x4919AA3: start_thread (pthread_create.c:447)
==11513==    by 0x49A6A63: clone (clone.S:100)
==11513== 
==11513== This conflicts with a previous read of size 8 by thread #2
==11513== Locks held: 1, at address 0x10C040
==11513==    at 0x109301: Radar (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x4919AA3: start_thread (pthread_create.c:447)
==11513==    by 0x49A6A63: clone (clone.S:100)
==11513==  Address 0x10c010 is 0 bytes inside data symbol "vida_Escudo"
==11513== 
==11513== ----------------------------------------------------------------
==11513== 
==11513==  Lock at 0x10C040 was first observed
==11513==    at 0x48512DC: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x1092FB: Radar (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x4919AA3: start_thread (pthread_create.c:447)
==11513==    by 0x49A6A63: clone (clone.S:100)
==11513==  Address 0x10c040 is 0 bytes inside data symbol "io_mutex"
==11513== 
==11513== Possible data race during read of size 8 at 0x10C010 by thread #2
==11513== Locks held: 1, at address 0x10C040
==11513==    at 0x109301: Radar (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x4919AA3: start_thread (pthread_create.c:447)
==11513==    by 0x49A6A63: clone (clone.S:100)
==11513== 
==11513== This conflicts with a previous write of size 8 by thread #3
==11513== Locks held: none
==11513==    at 0x109275: EscuadronDefensa (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x4919AA3: start_thread (pthread_create.c:447)
==11513==    by 0x49A6A63: clone (clone.S:100)
==11513==  Address 0x10c010 is 0 bytes inside data symbol "vida_Escudo"
==11513== 
 Integridad del escudo: 100050000 / 100000   ==11513== ---Thread-Announcement------------------------------------------
==11513== 
==11513== Thread #4 was created
==11513==    at 0x49A6A53: clone (clone.S:76)
==11513==    by 0x49A6BD2: __clone_internal_fallback (clone-internal.c:64)
==11513==    by 0x49A6BD2: __clone_internal (clone-internal.c:109)
==11513==    by 0x491954F: create_thread (pthread_create.c:297)
==11513==    by 0x491A1A4: pthread_create@@GLIBC_2.34 (pthread_create.c:836)
==11513==    by 0x4854975: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x10940A: main (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513== 
==11513== ----------------------------------------------------------------
==11513== 
==11513== Possible data race during read of size 8 at 0x10C010 by thread #4
==11513== Locks held: none
==11513==    at 0x1092B3: EscuadronAtaque (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x4919AA3: start_thread (pthread_create.c:447)
==11513==    by 0x49A6A63: clone (clone.S:100)
==11513== 
==11513== This conflicts with a previous write of size 8 by thread #3
==11513== Locks held: none
==11513==    at 0x109275: EscuadronDefensa (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x4919AA3: start_thread (pthread_create.c:447)
==11513==    by 0x49A6A63: clone (clone.S:100)
==11513==  Address 0x10c010 is 0 bytes inside data symbol "vida_Escudo"
==11513== 
==11513== ----------------------------------------------------------------
==11513== 
==11513==  Lock at 0x10C040 was first observed
==11513==    at 0x48512DC: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x1092FB: Radar (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x4919AA3: start_thread (pthread_create.c:447)
==11513==    by 0x49A6A63: clone (clone.S:100)
==11513==  Address 0x10c040 is 0 bytes inside data symbol "io_mutex"
==11513== 
==11513== Possible data race during write of size 8 at 0x10C010 by thread #4
==11513== Locks held: none
==11513==    at 0x1092BE: EscuadronAtaque (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x4919AA3: start_thread (pthread_create.c:447)
==11513==    by 0x49A6A63: clone (clone.S:100)
==11513== 
==11513== This conflicts with a previous read of size 8 by thread #2
==11513== Locks held: 1, at address 0x10C040
==11513==    at 0x109301: Radar (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x4919AA3: start_thread (pthread_create.c:447)
==11513==    by 0x49A6A63: clone (clone.S:100)
==11513==  Address 0x10c010 is 0 bytes inside data symbol "vida_Escudo"
==11513== 
 Integridad del escudo: 50000 / 100000   ==11513== ---Thread-Announcement------------------------------------------
==11513== 
==11513== Thread #5 was created
==11513==    at 0x49A6A53: clone (clone.S:76)
==11513==    by 0x49A6BD2: __clone_internal_fallback (clone-internal.c:64)
==11513==    by 0x49A6BD2: __clone_internal (clone-internal.c:109)
==11513==    by 0x491954F: create_thread (pthread_create.c:297)
==11513==    by 0x491A1A4: pthread_create@@GLIBC_2.34 (pthread_create.c:836)
==11513==    by 0x4854975: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x1093DD: main (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513== 
==11513== ----------------------------------------------------------------
==11513== 
==11513== Possible data race during read of size 8 at 0x10C010 by thread #5
==11513== Locks held: none
==11513==    at 0x10926A: EscuadronDefensa (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x4919AA3: start_thread (pthread_create.c:447)
==11513==    by 0x49A6A63: clone (clone.S:100)
==11513== 
==11513== This conflicts with a previous write of size 8 by thread #4
==11513== Locks held: none
==11513==    at 0x1092BE: EscuadronAtaque (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x4919AA3: start_thread (pthread_create.c:447)
==11513==    by 0x49A6A63: clone (clone.S:100)
==11513==  Address 0x10c010 is 0 bytes inside data symbol "vida_Escudo"
==11513== 
 Integridad del escudo: 50000 / 100000   ==11513== ---Thread-Announcement------------------------------------------
==11513== 
==11513== Thread #1 is the program's root thread
==11513== 
==11513== ----------------------------------------------------------------
==11513== 
==11513== Possible data race during write of size 1 at 0x10C018 by thread #1
==11513== Locks held: none
==11513==    at 0x109456: main (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513== 
==11513== This conflicts with a previous read of size 1 by thread #2
==11513== Locks held: none
==11513==    at 0x109347: Radar (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x4919AA3: start_thread (pthread_create.c:447)
==11513==    by 0x49A6A63: clone (clone.S:100)
==11513==  Address 0x10c018 is 0 bytes inside data symbol "mision_Activa"
==11513== 
==11513== ----------------------------------------------------------------
==11513== 
==11513== Possible data race during read of size 1 at 0x10C018 by thread #2
==11513== Locks held: none
==11513==    at 0x109347: Radar (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==11513==    by 0x4919AA3: start_thread (pthread_create.c:447)
==11513==    by 0x49A6A63: clone (clone.S:100)
==11513== 
==11513== This conflicts with a previous write of size 1 by thread #1
==11513== Locks held: none
==11513==    at 0x109456: main (in /home/c4c983/Documents/Lab 1 Hilos e Indeterminismo/lab-1-hilos-e-indeterminismo/Parte II/code)
==11513==  Address 0x10c018 is 0 bytes inside data symbol "mision_Activa"
==11513== 

 
Reporte final del escudo
Integridad proyectada: 50000
Integridad real: 50000
==11513== 
==11513== Use --history-level=approx or =none to gain increased speed, at
==11513== the cost of reduced accuracy of conflicting-access information
==11513== For lists of detected and suppressed errors, rerun with: -s
==11513== ERROR SUMMARY: 23 errors from 7 contexts (suppressed: 10 from 2)

Como se aprecia, hay un error de possible data race. El problema se debe a que multiples hilos estan intentando acceder a la variable vida_Escudo al mismo tiempo. Dos hilos cambian su valor, y uno la imprime.