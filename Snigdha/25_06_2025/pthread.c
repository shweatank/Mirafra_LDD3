#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int i=0;
// Thread function 1
void* task1(void* arg) {
    for (; i < 5; i++) {
        printf("Thread 1: Count %d\n", i + 1);
        sleep(1);
    }
    return NULL;
}

// Thread function 2
void* task2(void* arg) {
    for (; i < 3; i++) {
        printf("Thread 2: Count %d\n", i + 1);
        sleep(2);
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    printf("Main: Creating threads...\n");

    // Create threads
    pthread_create(&thread1, NULL, task1, NULL);
    pthread_create(&thread2, NULL, task2, NULL);

    // Wait for threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Main: Threads completed. Exiting program.\n");
    return 0;
}

