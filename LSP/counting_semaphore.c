#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_RESOURCES 3  // Only 3 resources available
#define NUM_PROCESSES 6  // 6 processes will compete for them

int main() {
    sem_t *sem;
    pid_t pid;
    int i;

    // Create or open a named semaphore with an initial value equal to the number of resources
    sem = sem_open("/counting_sem", O_CREAT, 0644, NUM_RESOURCES);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < NUM_PROCESSES; ++i) {
        pid = fork();
        if (pid == 0) { // Child process
            printf("Process %d: Trying to acquire a resource...\n", getpid());
            sem_wait(sem); // Decrement semaphore count; wait if count is 0
            
            printf("Process %d: Acquired a resource, doing work...\n", getpid());
            sleep(2); // Simulate working with the resource
            
            printf("Process %d: Releasing the resource.\n", getpid());
            sem_post(sem); // Increment semaphore count
            
            sem_close(sem);
            exit(EXIT_SUCCESS);
        } else if (pid < 0) { // Fork failed
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
    }

    // Parent waits for all child processes to finish
    for (i = 0; i < NUM_PROCESSES; ++i) {
        wait(NULL);
    }

    // Clean up: Close and unlink the named semaphore
    sem_close(sem);
    sem_unlink("/counting_sem");

    return 0;
}

