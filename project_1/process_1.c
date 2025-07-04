#include "shared.h"
pid_t pid2, pid3;
void sigint_handler(int sig)
{
    printf("SIGINT received, forwarding to Process 2 and 3\n");
    kill(pid2, SIGINT);
    kill(pid3, SIGINT);
}
void sigquit_handler(int sig) 
{
    printf("SIGQUIT received, forwarding to Process 2\n");
    kill(pid2, SIGQUIT);
}
int main()
{
    printf("Enter PID of Process 2: ");
    scanf("%d", &pid2);
    printf("Enter PID of Process 3: ");
    scanf("%d", &pid3);
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigquit_handler);
    printf("Signal Manager running PID: %d\n", getpid());
    printf("Press Ctrl+C to send SIGINT\n");
    printf("Press Ctrl+\\ to send SIGQUIT\n");
    printf("Enter 'q' and press Enter to quit and send SIGTERM\n");//to terminate p2 and p3
    char ch;
    while (1) 
    {
        ch = getchar();
        if (ch == 'q')
       	{
            printf("Sending SIGTERM to Process 2 and 3\n");
            kill(pid2, SIGTERM);
            kill(pid3, SIGTERM);
            break;
        }
    }
    return 0;
}

