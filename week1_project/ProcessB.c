#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include "shared_data.h"

void *reader_thread(void *arg) {
    struct SharedData *shared = (struct SharedData *)arg;

    while (1) {
        pthread_mutex_lock(&shared->mutex);
        while (shared->thread_turn != 0)
            pthread_cond_wait(&shared->cond, &shared->mutex);

        printf("Thread 1 (Reader): Read %d and %d\n", shared->num1, shared->num2);
        shared->thread_turn = 1;
        pthread_cond_signal(&shared->cond);
        pthread_mutex_unlock(&shared->mutex);
    }

    return NULL;
}

void *adder_thread(void *arg) {
    struct SharedData *shared = (struct SharedData *)arg;

    while (1) {
        pthread_mutex_lock(&shared->mutex);
        while (shared->thread_turn != 1)
            pthread_cond_wait(&shared->cond, &shared->mutex);

        shared->result = shared->num1 + shared->num2;
        printf("Thread 2 (Adder): Computed result = %d\n", shared->result);
        shared->thread_turn = -1;  // Lock threads

        pthread_mutex_unlock(&shared->mutex);
        sem_t *sem_read = sem_open(SEM_READ, 0);
        sem_post(sem_read);
        sem_close(sem_read);
    }

    return NULL;
}

int main() {
    // Open shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    struct SharedData *shared = mmap(NULL, sizeof(struct SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Open semaphore
    sem_t *sem_write = sem_open(SEM_WRITE, 0);

    // Initialize mutex and condition variable
    pthread_mutexattr_t mattr;
    pthread_condattr_t cattr;
    pthread_mutexattr_init(&mattr);
    pthread_condattr_init(&cattr);
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&shared->mutex, &mattr);
    pthread_cond_init(&shared->cond, &cattr);
    shared->thread_turn = 0;

    // Create threads
    pthread_t t1, t2;
    pthread_create(&t1, NULL, reader_thread, shared);
    pthread_create(&t2, NULL, adder_thread, shared);

    // Main loop
    for (int i = 0; i < 5; ++i) {
        sem_wait(sem_write); // Wait for child A

        pthread_mutex_lock(&shared->mutex);
        shared->thread_turn = 0;
        pthread_cond_signal(&shared->cond);  // Start thread 1
        pthread_mutex_unlock(&shared->mutex);
    }

    pthread_cancel(t1);
    pthread_cancel(t2);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    munmap(shared, sizeof(struct SharedData));
    close(shm_fd);

    return 0;
}

