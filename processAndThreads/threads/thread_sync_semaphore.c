#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Global variable to be incremented
int global_counter = 0;

// Mutex to protect access to the global variable
pthread_mutex_t counter_mutex;

// Function to be executed by each thread
void* increment_counter(void* arg) {
    int i;
    for (i = 0; i < 100000; i++) { // Increment 100,000 times
        pthread_mutex_lock(&counter_mutex); // Acquire the mutex lock
        global_counter++;                   // Increment the global variable
        pthread_mutex_unlock(&counter_mutex); // Release the mutex lock
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    // Initialize the mutex
    if (pthread_mutex_init(&counter_mutex, NULL) != 0) {
        perror("Mutex initialization failed");
        return 1;
    }

    // Create the first thread
    if (pthread_create(&thread1, NULL, increment_counter, NULL) != 0) {
        perror("Failed to create thread 1");
        return 1;
    }

    // Create the second thread
    if (pthread_create(&thread2, NULL, increment_counter, NULL) != 0) {
        perror("Failed to create thread 2");
        return 1;
    }

    // Wait for both threads to finish
    if (pthread_join(thread1, NULL) != 0) {
        perror("Failed to join thread 1");
        return 1;
    }
    if (pthread_join(thread2, NULL) != 0) {
        perror("Failed to join thread 2");
        return 1;
    }

    // Destroy the mutex
    pthread_mutex_destroy(&counter_mutex);

    printf("Final value of global_counter: %d\n", global_counter);

    return 0;
}
