#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include<semaphore.h>
// Global counter variable
int counter = 0;
sem_t sem;
// Mutex to prevent race conditions
//pthread_mutex_t lock;

// Thread function to increment the counter
void* increment_thread(void* arg) {
   for (int i = 0; i < 10; i++) {
	   sem_wait(&sem);
        counter++;
	sem_post(&sem);
    }
    return NULL;
}


int main() {
    pthread_t t1, t2;

    // Initialize mutex
   // pthread_mutex_init(&lock, NULL);
     sem_init(&sem,0,1);
    // Create threads
    pthread_create(&t1, NULL, increment_thread, NULL);
    pthread_create(&t2, NULL, increment_thread, NULL);

    // Wait for threads to complete
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Destroy mutex
   // pthread_mutex_destroy(&lock);

    printf("Final counter value: %d\n", counter);

    return 0;
}

