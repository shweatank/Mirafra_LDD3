#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handle_sigint(int sig)
{
    printf("\nHandled signal %d usinf sigactipon\n", sig);
    _exit(0);
}

int main()
{
    struct sigaction sa;

    sa.sa_handler = handle_sigint;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGINT, &sa, NULL);

    while (1) {
        printf("Running... Press Ctrl+C to stop\n");
        sleep(1);
    }

    return 0;
}

