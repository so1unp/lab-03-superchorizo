#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/wait.h>

int busywork(void)
{
    struct tms buf;
    while (1)
    {
        times(&buf);
    }
}

void sigterm_handler(int sig)
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    int prio = getpriority(PRIO_PROCESS, 0);
    printf("Child %d (nice %2d):\t%ld.%06ld\n", getpid(), prio, usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Uso: %s <num_procesos> <segundos> <reducir_prioridad>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    int secs = atoi(argv[2]);
    int reduce = atoi(argv[3]);

    if (n <= 0)
    {
        fprintf(stderr, "Número de procesos inválido.\n");
        exit(EXIT_FAILURE);
    }

    pid_t *children = malloc(sizeof(pid_t) * n);
    if (children == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    struct sigaction sa;
    sa.sa_handler = sigterm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    for (int i = 0; i < n; i++)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            if (reduce)
            {
                int new_nice = i * 5;
                setpriority(PRIO_PROCESS, 0, new_nice);
            }
            sigaction(SIGTERM, &sa, NULL);
            busywork();
            exit(EXIT_SUCCESS);
        }
        else
        {
            children[i] = pid;
        }
    }

    if (secs > 0)
    {
        sleep(secs);
        for (int i = 0; i < n; i++)
        {
            kill(children[i], SIGTERM);
        }
        for (int i = 0; i < n; i++)
        {
            waitpid(children[i], NULL, 0);
        }
    }
    else
    {
        while (1)
        {
            pause();
        }
    }

    free(children);
    return EXIT_SUCCESS;
}
