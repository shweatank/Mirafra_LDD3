// signal_manager.c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
pid_t pid2, pid3;
void sigint_handler(int sig)
{
    printf("signal_manager: SIGINT received, forwarding to p2 and p3\n");
    kill(pid2, SIGINT);
    kill(pid3, SIGINT);
}
void sigquit_handler(int sig)
{
    printf("signal_manager: SIGQUIT received, forwarding to p2\n");
    kill(pid2, SIGQUIT);
}
int main()
{
    printf("Enter PID of process2: ");
    scanf("%d", &pid2);
    printf("Enter PID of process3: ");
    scanf("%d", &pid3);
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigquit_handler);
    printf("signal_manager running, press Ctrl+C to send SIGINT\n");
    printf("Press Ctrl+\\ to send SIGQUIT\n");
    printf("Press 'q' then Enter to quit and send SIGTERM to p2 and p3\n");
    char ch;
    while (1)
    {
        ch = getchar();
        if (ch == 'q')
        {
            printf("signal_manager: sending SIGTERM to p2 and p3\n");
            kill(pid2, SIGTERM);
            kill(pid3, SIGTERM);
            break;
        }
    }
    return 0;
}

