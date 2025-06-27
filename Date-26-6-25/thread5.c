#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#define THREAD_COUNT 5

sem_t sem;  

void* thread_function(void* arg) {
    int id = *((int*)arg);
    free(arg);

    printf("Thread %d waiting to enter...\n", id);
    sem_wait(&sem);  

    printf("Thread %d entered the room.\n", id);
    sleep(2);  
    printf("Thread %d leaving the room.\n", id);

    sem_post(&sem);  
    return NULL;
}

int main() {
    pthread_t threads[THREAD_COUNT];

    sem_init(&sem, 0, 2);

    for (int i = 0; i < THREAD_COUNT; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&threads[i], NULL, thread_function, id);
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&sem); 
    return 0;
}

