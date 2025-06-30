#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>


int main() 
{
    pid_t pid;

    // Create a child process
    pid = fork();

    if (pid < 0) {
        // Error occurred
        fprintf(stderr, "E: fork failed.\n");
        return 1;
    }
    else if (pid == 0) {
        // Child process
    	execlp("/bin/ls", "ls", NULL);
    }
    else {
        // Parent process
    	wait(NULL);
	printf("I: Child completed.\n");
    }

    return 0;
}

