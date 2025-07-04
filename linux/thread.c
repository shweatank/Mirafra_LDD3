#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int shared_var = 1;
pthread_mutex_t lock;
void* increment(void* arg) 
{
    for (int i = 0; i < 5; i++)
    {
	pthread_mutex_lock(&lock);
        shared_var += 1;
        printf("Thread_1_Increment:shared_var = %d\n", shared_var);
        sleep(1);
	pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void* multiply(void* arg) {
    for (int i = 0; i < 5; i++) 
    {
	pthread_mutex_lock(&lock);
        shared_var *= 2;
        printf("Thread_2_Multiply: shared_var = %d\n", shared_var);
        sleep(1);
	pthread_mutex_unlock(&lock);
    }
    return NULL;
}
int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, multiply, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Final value of shared_var: %d\n", shared_var);
    return 0;
}

