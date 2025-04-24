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

    if (hijos <= 0)
    {
        fprintf(stderr, "Número de procesos inválido.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < hijos; i++)
    { 

        pid_t pid = fork();

        if (pid == 0) {       
            printf("Soy el hijo %d, PID: %d\n", i, getpid());
            
            
            exit(EXIT_SUCCESS);
        }
        while (1)
        {
            pause();
        }
        
    }



    exit(EXIT_SUCCESS);
}
