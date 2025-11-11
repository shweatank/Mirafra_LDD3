#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int shared_data = 0;       // Shared variable
sem_t sem_produce, sem_consume;

void *producer(void *arg)
{
    for (int i = 1; i <= 5; i++)
    {
        sem_wait(&sem_produce);      // Wait if consumer hasn't consumed
        shared_data = i;
        printf("Produced: %d\n", shared_data);
        sem_post(&sem_consume);      // Signal consumer
        sleep(1);
    }
    return NULL;
}

void *consumer(void *arg)
{
    for (int i = 1; i <= 5; i++)
    {
        sem_wait(&sem_consume);      // Wait until producer produces
        printf("Consumed: %d\n", shared_data);
        sem_post(&sem_produce);      // Signal producer
        sleep(1);
    }
    return NULL;
}

int main()
{
    pthread_t t1, t2;

    sem_init(&sem_produce, 0, 1);   // Producer can start immediately
    sem_init(&sem_consume, 0, 0);   // Consumer waits initially

    pthread_create(&t1, NULL, producer, NULL);
    pthread_create(&t2, NULL, consumer, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    sem_destroy(&sem_produce);
    sem_destroy(&sem_consume);

    return 0;
}

