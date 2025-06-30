#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // For sleep()

// Global variable to be modified by threads
int global_variable = 0;

// Counting semaphore for synchronization
sem_t counting_semaphore;

// Function executed by each thread
void *thread_function(void *arg) {
    int thread_id = *(int *)arg;

    for (int i = 0; i < 5; ++i) { // Each thread modifies the variable 5 times
        // Wait on the semaphore (decrement its value)
        // This ensures only one thread can access the critical section at a time
        sem_wait(&counting_semaphore);

        // Critical section: modify the global variable
        global_variable++;
        printf("Thread %d: global_variable = %d\n", thread_id, global_variable);

        // Post on the semaphore (increment its value)
        // This allows another waiting thread to enter the critical section
        sem_post(&counting_semaphore);

        // Simulate some work outside the critical section
        usleep(100000); // Sleep for 100 milliseconds
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[3];
    int thread_ids[3];

    // Initialize the counting semaphore with an initial value of 1
    // This makes it a binary semaphore, allowing only one thread in the critical section
    sem_init(&counting_semaphore, 0, 1);

    // Create three threads
    for (int i = 0; i < 3; ++i) {
        thread_ids[i] = i + 1;
        if (pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // Join threads (wait for them to complete)
    for (int i = 0; i < 3; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
            return 1;
        }
    }

    // Destroy the semaphore
    sem_destroy(&counting_semaphore);

    printf("Final value of global_variable: %d\n", global_variable);

    return 0;
}
