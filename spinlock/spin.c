#include <stdio.h>
#include <pthread.h>

#define NUM_ITERATION 1000000

int counter = 0;
pthread_spinlock_t spinlock;

void* increment_counter(void* arg)
{
    for (int i = 0; i < NUM_ITERATION; i++)
    {
        pthread_spin_lock(&spinlock);
        counter++;
        pthread_spin_unlock(&spinlock);
    }
    return NULL;
}

int main()
{
    pthread_t t1, t2;

    pthread_spin_init(&spinlock, 0);

    pthread_create(&t1, NULL, increment_counter, NULL);
    pthread_create(&t2, NULL, increment_counter, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_spin_destroy(&spinlock);

    printf("Final counter value: %d\n", counter);
    return 0;
}

