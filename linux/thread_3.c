#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t count_sem;

void* task(void* arg) {
    int id = *(int*)arg;

    printf("Thread %d waiting for counting semaphore...\n", id);
    sem_wait(&count_sem);
    printf("Thread %d entered critical section\n", id);
    sleep(2);
    printf("Thread %d leaving critical section\n", id);
    sem_post(&count_sem);

    return NULL;
}

int main() {
    pthread_t t[3];
    int id[3] = {1, 2, 3};

    sem_init(&count_sem, 0, 2);  // Count = 2, so 2 threads allowed at a time

    for (int i = 0; i < 3; i++)
        pthread_create(&t[i], NULL, task, &id[i]);

    for (int i = 0; i < 3; i++)
        pthread_join(t[i], NULL);

    sem_destroy(&count_sem);
    return 0;
}

