#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int add(int a, int b)
{
	return a+b;
}

int main() 
{
    pid_t pid;

    int ans = 0;

    // Create a child process
    pid = fork();

    if (pid < 0) {
        // Error occurred
        perror("Fork failed");
        return 1;
    }
    else if (pid == 0) {
        // Child process
	printf("This is child\n");
	printf("  ans = %d\n", ans);
        printf("  PID: %d\n", getpid());
        printf("  Parent PID: %d\n", getppid());
    }
    else {
        // Parent process
	printf("This is Parent\n");
        ans = add(3, 5);
	printf("  PID: %d\n", getpid());
        printf("  Child PID: %d\n", pid);
    }

    return 0;
}

