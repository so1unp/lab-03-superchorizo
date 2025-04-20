#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

// Handler para las señales
void signal_handler(int signo) {
    printf("%d: %s\n", signo, strsignal(signo));
    fflush(stdout); // Asegura que el mensaje se imprima inmediatamente
}

int main() {
    struct sigaction sa;

    // Configurar el handler para todas las señales
    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    // Registrar el handler para todas las señales posibles
    for (int i = 1; i < _NSIG; i++) {
        if (i == SIGKILL || i == SIGSTOP || i == 32 || i == 33) {
            continue; // Omitir señales que no pueden ser manejadas
        }
        if (sigaction(i, &sa, NULL) == -1) {
            fprintf(stderr, "No se pudo manejar la señal %d: %s\n", i, strerror(errno));
        }
    }

    // Esperar señales indefinidamente
    while (1) {
        pause(); // Espera a que llegue una señal
    }

    return 0;
} 
/*Para implementar el programa sigcatch.c, puedes usar el siguiente código como base.
 Este programa intercepta señales, imprime su identificador y descripción, y espera continuamente señales usando pause().
Explicación del código:
sigaction:

Configura un manejador de señales (signal_handler) para todas las señales posibles.
Usa un bucle para registrar el manejador para todas las señales desde 1 hasta _NSIG - 1.
signal_handler:

Imprime el número de la señal (signo) y su descripción usando strsignal.
pause:

Suspende la ejecución del programa hasta que se reciba una señal.
Errores:

Algunas señales no pueden ser atrapadas (como SIGKILL y SIGSTOP). Esto se maneja con un mensaje de error.
Cuáles son las señales que no es posible atrapar? ¿Por qué?

SIGKILL: No puede ser atrapada, bloqueada ni ignorada porque es utilizada por el kernel para forzar la terminación de un proceso.
SIGSTOP: No puede ser atrapada ni ignorada porque es utilizada para detener un proceso de manera incondicional.

Explicación del cambio:
continue: Salta las señales SIGKILL y SIGSTOP en el bucle.
Esto evita intentar registrar un manejador para señales que no pueden ser atrapadas.
Resultado esperado:
Al ejecutar el programa nuevamente, no deberías ver mensajes de error para SIGKILL y SIGSTOP.
 Sin embargo, los mensajes para otras señales como SIGRTMIN pueden seguir apareciendo dependiendo de tu sistema.
Solución para omitir estas señales:
Puedes agregar una condición adicional en el bucle para ignorar las señales 32 y 33, además de SIGKILL y SIGSTOP.

Explicación del cambio:
i == 32 || i == 33:

Estas condiciones evitan intentar registrar un manejador para las señales 32 y 33.
Esto elimina los mensajes de error relacionados con estas señales.
continue:

Salta la iteración del bucle para estas señales, asegurando que no se intente manejarlas.
Resultado esperado:
Al ejecutar el programa nuevamente, no deberías ver mensajes de error para las señales 32 y 33. 
El programa continuará manejando las demás señales correctamente.
*/