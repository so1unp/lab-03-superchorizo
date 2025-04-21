#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void test_fork(int count, int do_wait);
void test_thread(int count, int do_wait);

int main(int argc, char *argv[])
{
    int modo, do_wait, count;

    // Chequea los  parametros
    if (argc < 3)
    {
        fprintf(stderr, "Uso: %s [-p | -t] [-w] cantidad\n", argv[0]);
        fprintf(stderr, "\t-p           Crear procesos.\n");
        fprintf(stderr, "\t-t           Crear hilos.\n");
        fprintf(stderr, "\t-w           Esperar que proceso o hilo finalice.\n");
        fprintf(stderr, "\tcantidad     Número de procesos o hilos a crear.\n");
        exit(EXIT_FAILURE);
    }

    modo = argv[1][1]; // debe ser 'p' o 't'

    if (argc == 4)
    {
        if (argv[2][1] != 'w')
        {
            fprintf(stderr, "Error: opción invalida %s.\n", argv[2]);
            exit(EXIT_FAILURE);
        }
        do_wait = 1;
        count = atoi(argv[3]);
    }
    else
    {
        do_wait = 0;
        count = atoi(argv[2]);
    }

    if (count <= 0)
    {
        fprintf(stderr, "Error: el contador debe ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    if (modo != 'p' && modo != 't')
    {
        fprintf(stderr, "Error: opción invalida %s.\nUsar -p (procesos) o -t (hilos)\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if (modo == 'p')
    {
        printf("Probando fork()...\n");
        test_fork(count, do_wait);
    }
    else if (modo == 't')
    {
        printf("Probando pthread_create()...\n");
        test_thread(count, do_wait);
    }

    exit(EXIT_SUCCESS);
}
void *thread_func(void *arg)
{
    (void)arg; // evita warning por parámetro no usado
    pthread_exit(NULL);
}
//==========================================================================
// Código para la prueba con fork()
//==========================================================================
void test_fork(int count, int do_wait)
{
    for (int i = 0; i < count; i++)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            _exit(EXIT_SUCCESS); // Hijo sale
        }
        else
        {
            if (do_wait)
            {
                waitpid(pid, NULL, 0);
            }
        }
    }
    if (!do_wait)
    {
        for (int i = 0; i < count; i++)
        {
            wait(NULL);
        }
    }
}
//==========================================================================
// Código para la prueba con pthread_create()
//==========================================================================
void test_thread(int count, int do_wait)
{
    pthread_t *threads = malloc((size_t)count * sizeof(pthread_t));
    if (!threads)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < count; i++)
    {
        if (pthread_create(&threads[i], NULL, thread_func, NULL) != 0)
        {
            perror("pthread_create");
            free(threads);
            exit(EXIT_FAILURE);
        }

        if (do_wait)
        {
            pthread_join(threads[i], NULL);
        }
    }

    if (!do_wait)
    {
        for (int i = 0; i < count; i++)
        {
            pthread_join(threads[i], NULL);
        }
    }

    free(threads);
}
