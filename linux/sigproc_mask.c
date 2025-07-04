#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main() 
{
    sigset_t set;

    // Initialize the signal set to empty
    sigemptyset(&set);

    // Add SIGINT to the set (Ctrl+C)
    sigaddset(&set, SIGINT);

    // Block SIGINT
    sigprocmask(SIG_BLOCK, &set, NULL);

    printf("SIGINT is blocked for 10 seconds. Try Ctrl+C now...\n");
    sleep(10);

    // Unblock SIGINT
    sigprocmask(SIG_UNBLOCK, &set, NULL);

    printf("SIGINT unblocked. Now Ctrl+C will work again.\n");

    while (1)
    {
        sleep(1);
    }

    return 0;
}

