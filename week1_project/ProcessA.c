#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>
#include <sys/wait.h>
#include "shared_data.h"

volatile sig_atomic_t start_loop = 0;
volatile sig_atomic_t print_result = 0;

void handle_sigint(int signo) {
    start_loop = 1;
}

void handle_sigquit(int signo) {
    print_result = 1;
}

int main() {
    signal(SIGINT, handle_sigint);     // Ctrl+C
    signal(SIGQUIT, handle_sigquit);   // Ctrl+\

    int array[] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    int total_pairs = 5;

    // Create shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(struct SharedData));
    struct SharedData *shared = mmap(NULL, sizeof(struct SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Create semaphores
    sem_t *sem_write = sem_open(SEM_WRITE, O_CREAT, 0666, 0);
    sem_t *sem_read = sem_open(SEM_READ, O_CREAT, 0666, 0);
    sem_t *sem_next_iter = sem_open(SEM_ITER, O_CREAT, 0666, 1);  // initially allow iteration 1

    pid_t pid = fork();

    if (pid == 0) {
        // ---- Child: write two numbers ----
        for (int i = 0; i < total_pairs; ++i) {
            sem_wait(sem_next_iter);  // Wait for parent to allow next iteration

            printf("Child A: Waiting for Ctrl+C to start iteration %d...\n", i + 1);
            while (!start_loop) pause();  // Wait for user signal (Ctrl+C)
            start_loop = 0;

            shared->num1 = array[2 * i];
            shared->num2 = array[2 * i + 1];
            printf("Child A: Written %d and %d to shared memory\n", shared->num1, shared->num2);

            sem_post(sem_write); // Signal B to proceed
        }
        exit(0);
    } else {
        // ---- Parent: read and print result ----
        for (int i = 0; i < total_pairs; ++i) {
            sem_wait(sem_read); // Wait for B to finish calculation

            printf("Parent A: Waiting for Ctrl+\\ to print result of iteration %d...\n", i + 1);
            while (!print_result) pause();  // Wait for user signal (Ctrl+\)
            print_result = 0;

            printf("Parent A: Received result = %d\n", shared->result);

            sem_post(sem_next_iter); // Allow child to proceed
        }

        wait(NULL);  // Wait for child to exit

        // Cleanup
        sem_close(sem_write);
        sem_close(sem_read);
        sem_close(sem_next_iter);
        sem_unlink(SEM_WRITE);
        sem_unlink(SEM_READ);
        sem_unlink("/sem_next_iter");

        munmap(shared, sizeof(struct SharedData));
        shm_unlink(SHM_NAME);

        return 0;
    }
}

