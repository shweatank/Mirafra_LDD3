#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS 5

sem_t sem;  // counting semaphore

void* threadFunc(void* arg) {
    int threadNum = *(int*)arg;

    printf("Thread %d: Waiting to enter...\n", threadNum);
    sem_wait(&sem);  // Decrement the semaphore
    printf("Thread %d: Entered critical section.\n", threadNum);

    sleep(2);  // Simulate work in critical section

    printf("Thread %d: Leaving critical section.\n", threadNum);
    sem_post(&sem);  // Increment the semaphore

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int threadIds[NUM_THREADS];

    // Initialize the semaphore to 3 (i.e., 3 threads can enter at a time)
    sem_init(&sem, 0, 3);  // 0 = shared between threads (not processes)

    for (int i = 0; i < NUM_THREADS; i++) {
        threadIds[i] = i + 1;
        pthread_create(&threads[i], NULL, threadFunc, &threadIds[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destroy the semaphore
    sem_destroy(&sem);

    return 0;
}

