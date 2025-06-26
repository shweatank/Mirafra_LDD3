#include <stdio.h>
#include <pthread.h>

int counter = 0;                    // Shared variable
pthread_mutex_t lock;              // Mutex declaration

void* increment(void* arg) {
    for(int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&lock); // Lock the mutex
        counter++;                 // Critical section
        pthread_mutex_unlock(&lock); // Unlock the mutex
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    // Initialize the mutex
    pthread_mutex_init(&lock, NULL);

    // Create two threads
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);

    // Wait for both threads to finish
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Destroy the mutex
    pthread_mutex_destroy(&lock);

    // Print final counter value
    printf("Final counter value: %d\n", counter);
    return 0;
}
