#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sem;

void* task(void* arg)
{
    sem_wait(&sem);
    printf("Thread %ld entered\n", (long)arg);
    sleep(1);
    printf("Thread %ld exited\n", (long)arg);
    sem_post(&sem);
    return NULL;
}

int main()
{
    pthread_t t1, t2, t3;
    sem_init(&sem, 0, 2);

    pthread_create(&t1, NULL, task, (void*)1);
    pthread_create(&t2, NULL, task, (void*)2);
    pthread_create(&t3, NULL, task, (void*)3);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    sem_destroy(&sem);
    return 0;
}

