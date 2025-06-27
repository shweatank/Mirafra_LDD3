#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Global counter variable
int counter = 0;

// Mutex to prevent race conditions
//pthread_mutex_t lock;

// Thread function to increment the counter
void* increment_thread(void* arg) {
//    for (int i = 0; i < 10; i++) {
  //      pthread_mutex_lock(&lock);
        counter++;
        printf("Increment thread: counter = %d\n", counter);
    //    pthread_mutex_unlock(&lock);
  //      sleep(1);
   // }
    return NULL;
}

// Thread function to decrement the counter
void* decrement_thread(void* arg) {
   // for (int i = 0; i < 10; i++) {
      //  pthread_mutex_lock(&lock);
        counter--;
        printf("Decrement thread: counter = %d\n", counter);
       // pthread_mutex_unlock(&lock);
     //   sleep(1);
   // }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    // Initialize mutex
   // pthread_mutex_init(&lock, NULL);

    // Create threads
    pthread_create(&t1, NULL, increment_thread, NULL);
    pthread_create(&t2, NULL, decrement_thread, NULL);

    // Wait for threads to complete
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Destroy mutex
   // pthread_mutex_destroy(&lock);

    printf("Final counter value: %d\n", counter);

    return 0;
}

