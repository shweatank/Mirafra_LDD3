#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig)
{
    printf("Signal %d received\n", sig);
}

int main()
{
    struct sigaction sa;
    sigset_t block_set;

    sa.sa_handler = handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);

    printf("Blocking SIGINT for 5 seconds. Try pressing Ctrl+C...\n");
    sigprocmask(SIG_BLOCK, &block_set, NULL);

    sleep(5);

    printf("Unblocking SIGINT now.\n");
    sigprocmask(SIG_UNBLOCK, &block_set, NULL);

    while (1)
    {
	    pause();
		    
     }     

    return 0;
}

