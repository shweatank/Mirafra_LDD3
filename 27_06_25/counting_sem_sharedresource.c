#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define THREAD_COUNT 3

sem_t sem;          // Counting semaphore
int shared_var = 0; // Common shared variable

void* thread_func(void* arg) {
    int id = *(int*)arg;

    printf("Thread %d: waiting to access shared resource\n", id);

    sem_wait(&sem);  // Acquire semaphore (may block)

    // 🔐 Critical section starts
    printf("Thread %d: entered critical section\n", id);
    int temp = shared_var;
    sleep(1); // Simulate some work
    temp += 1;
    shared_var = temp;
    printf("Thread %d: updated shared_var = %d\n", id, shared_var);
    printf("Thread %d: exiting critical section\n", id);
    // 🔐 Critical section ends

    sem_post(&sem);  // Release semaphore

    return NULL;
}

int main() {
    pthread_t threads[THREAD_COUNT];
    int ids[THREAD_COUNT] = {1, 2, 3};

    // Initialize counting semaphore with value 2
    sem_init(&sem, 0, 2);

    // Create threads
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    // Final value of shared_var
    printf("\nFinal value of shared_var: %d\n", shared_var);

    // Clean up
    sem_destroy(&sem);

    return 0;
}

