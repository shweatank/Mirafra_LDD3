#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main() 
{
    pid_t pid;
    printf("Enter the PID of receiver process (parent in file2): ");
    scanf("%d", &pid);

    if (kill(pid, SIGUSR1) == 0) 
    {
        printf("Signal sent to process %d\n", pid);
    } 
    else 
    {
        perror("Failed to send signal");
    }

    return 0;
}

