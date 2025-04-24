#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>      // times()

int busywork(void)
{
    struct tms buf;
    for (;;) {
        times(&buf);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Uso: %s <num_procesos> <segundos> <reducir_prioridad>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int hijos = atoi(argv[1]);       //número de hijos
    int segs = atoi(argv[2]);        //segundos
    int prio = atoi(argv[3]);        //activar prioridad


    exit(EXIT_SUCCESS);
}