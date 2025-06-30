#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int gVar = 5;

int main() 
{
    int lVar = 8;
    pid_t pid;

    // Create a child process
    pid = fork();

    if (pid < 0) {
        // Error occurred
        perror("Fork failed");
        return 1;
    }
    else if (pid == 0) {
        // Child process
        printf("Child Process: gVar = %d, lVar = %d\n", gVar, lVar);
        printf("Child Process: addr--> &gVar = %p, &lVar = %p\n", &gVar, &lVar);
        printf("  PID: %d\n", getpid());
        printf("  Parent PID: %d\n", getppid());
    }
    else {
        // Parent process
        printf("Parent Process: gVar = %d, lVar = %d\n", gVar, lVar);
        printf("Parent Process: addr--> &gVar = %p, &lVar = %p\n", &gVar, &lVar);
        printf("  PID: %d\n", getpid());
        printf("  Child PID: %d\n", pid);
    }

    return 0;
}

