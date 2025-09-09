// compile with: gcc -pthread sem_processes.c -o sem_processes

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/wait.h>

int main() {
    // Allocate shared memory for semaphore
    sem_t *sem = mmap(NULL, sizeof(sem_t), 
                      PROT_READ | PROT_WRITE, 
                      MAP_SHARED | MAP_ANONYMOUS, 
                      -1, 0);
    if (sem == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Initialize semaphore for process-shared use
    sem_init(sem, 1, 1); // pshared = 1 (process-shared), value = 1

    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        sem_wait(sem);
        printf("Child: In critical section\n");
        sleep(2);
        printf("Child: Leaving critical section\n");
        sem_post(sem);
        exit(0);
    } else {
        // Parent process
        sem_wait(sem);
        printf("Parent: In critical section\n");
        sleep(2);
        printf("Parent: Leaving critical section\n");
        sem_post(sem);

        wait(NULL); // Wait for child to finish

        // Cleanup
        sem_destroy(sem);
        munmap(sem, sizeof(sem_t));
    }

    return 0;
}

