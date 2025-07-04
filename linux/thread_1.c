#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

int shared_var = 1;
sem_t sem;  // binary semaphore

void* increment(void* arg)
{
    for (int i = 0; i < 5; i++)
    {
        sem_wait(&sem);  // wait (P operation)
        shared_var += 1;
        printf("Thread_1_Increment: shared_var = %d\n", shared_var);
        sleep(1);
        sem_post(&sem);  // signal (V operation)
    }
    return NULL;
}

void* multiply(void* arg)
{
    for (int i = 0; i < 5; i++)
    {
        sem_wait(&sem);
        shared_var *= 2;
        printf("Thread_2_Multiply: shared_var = %d\n", shared_var);
        sleep(1);
        sem_post(&sem);
    }
    return NULL;
}

int main()
{
    pthread_t t1, t2;

    // Initialize binary semaphore to 1 (available)
    sem_init(&sem, 0, 1);

    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, multiply, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final value of shared_var: %d\n", shared_var);

    sem_destroy(&sem);
    return 0;
}

