#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void handler(int sig) {
    char msg[100];
    int len = snprintf(msg, sizeof(msg), "%d: %s\n", sig, strsignal(sig));
    write(STDOUT_FILENO, msg, (size_t)len); 
}

int main() {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    for (int i = 1; i < _NSIG; i++) {
        sigaction(i, &sa, NULL); 
    }

    printf("PID: %d\n", getpid());

    while (1) {
        pause();
    }

    return EXIT_SUCCESS;
}
