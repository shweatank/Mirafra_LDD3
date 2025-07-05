#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define THREAD_COUNT 3

sem_t sem;  // Counting semaphore

void* thread_func(void* arg) {
    int id = *(int*)arg;

    printf("Thread %d: waiting for permission\n", id);
    sem_wait(&sem);  // Wait (decrement)
    
    // Critical Section
    printf("Thread %d: entered critical section\n", id);
    sleep(2); // Simulate work
    printf("Thread %d: exiting critical section\n", id);

    sem_post(&sem);  // Signal (increment)
    return NULL;
}

int main() {
    pthread_t threads[THREAD_COUNT];
    int ids[THREAD_COUNT] = {1, 2, 3};

    // Initialize counting semaphore to 2 (only 2 threads allowed at a time)
    sem_init(&sem, 0, 2);

    // Create threads
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }

    // Join threads
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    // Clean up
    sem_destroy(&sem);

    return 0;
}

