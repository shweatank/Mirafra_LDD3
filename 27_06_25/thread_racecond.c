#include <stdio.h>
#include <pthread.h>

#define NUM_INCREMENTS 100000

int counter = 0;

void* increment(void* arg) {
    for (int i = 0; i < NUM_INCREMENTS; i++) {
        counter++; // Unsafe access
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final counter (without mutex): %d\n", counter); // Often < 200000
    return 0;
}

