#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main()
{
    sigset_t set, pending;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigprocmask(SIG_BLOCK, &set, NULL);

    printf("SIGINT is blocked for 10 seconds. Try Ctrl+C now...\n");

    sleep(5);

    // Check pending signals
    sigpending(&pending);
    if (sigismember(&pending, SIGINT))
        printf("SIGINT is pending...\n");

    sleep(5);

    // Unblock
    sigprocmask(SIG_UNBLOCK, &set, NULL);
    printf("SIGINT unblocked. Now Ctrl+C will work again.\n");

    while (1)
        sleep(1);

    return 0;
}

