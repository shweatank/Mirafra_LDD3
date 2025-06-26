#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sem;

void* worker(void* arg) {
    sem_wait(&sem);  // wait (decrease) the semaphore
    printf("Thread %ld is in critical section\n", (long)arg);
    sleep(1);
    printf("Thread %ld is leaving critical section\n", (long)arg);
    sem_post(&sem);  // signal (increase) the semaphore
    return NULL;
}

int main() {
    pthread_t threads[4];
    
    sem_init(&sem, 0, 3); // 0: shared between threads, 1: initial value

    for (long i = 0; i < 4; ++i) {
        pthread_create(&threads[i], NULL, worker, (void*)i);
    }

    for (int i = 0; i < 4; ++i) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&sem);
    return 0;
}
