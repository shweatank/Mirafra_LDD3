#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS 5

sem_t sem;  // Counting semaphore

void* task(void* arg) {
    int id = *(int*)arg;

    printf("Thread %d: waiting to enter\n", id);
    sem_wait(&sem);  // Wait (decrement)
    
    printf("Thread %d: entered critical section\n", id);
    sleep(2); // Simulate work (e.g., printing)
    
    printf("Thread %d: leaving critical section\n", id);
    sem_post(&sem);  // Signal (increment)

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    // Initialize semaphore to 2 (only 2 threads allowed at a time)
    sem_init(&sem, 0, 2);

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, task, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&sem);  // Clean up
    return 0;
}

