#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#define _GNU_SOURCE

int busywork(void)
{
    struct tms buf;
    for (;;)
    {
        times(&buf);
    }
}

// Handler para los hijos
void terminar(int sig)
{
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    int prio = getpriority(PRIO_PROCESS, 0);
    printf("Child %d (nice %2d):\t%3li\n", getpid(), prio, ru.ru_utime.tv_sec);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Uso: %s <num_procesos> <segundos> <reducir_prioridad>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int hijos = atoi(argv[1]);
    int segs = atoi(argv[2]);
    int reducir = atoi(argv[3]);

    if (hijos <= 0)
    {
        fprintf(stderr, "Número de procesos inválido.\n");
        exit(EXIT_FAILURE);
    }

    pid_t *pids = malloc(sizeof(pid_t) * hijos);
    if (!pids)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < hijos; i++)
    {
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        // Proceso hijo
        if (pid == 0)
        {
            struct sigaction sa;
            sa.sa_handler = terminar;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sigaction(SIGTERM, &sa, NULL);

            if (reducir)
            {
                if (setpriority(PRIO_PROCESS, 0, i) != 0)
                {
                    fprintf(stderr, "Error al cambiar prioridad: %s\n", strerror(errno));
                }
            }

            busywork();
            exit(EXIT_SUCCESS);
        } // Proceso padre
        else
        {
            pids[i] = pid;
        }
    }

    // Esperar el tiempo especificado
    if (segs == 0)
    {
        pause(); // Infinito hasta Ctrl + C
    }
    else
    {
        sleep(segs);
    }

    // Terminar hijos
    for (int i = 0; i < hijos; i++)
    {
        kill(pids[i], SIGTERM);
    }

    // Esperar a que todos los hijos terminen
    for (int i = 0; i < hijos; i++)
    {
        waitpid(pids[i], NULL, 0);
    }

    free(pids);
    return 0;
}
