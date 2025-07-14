#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define NUM_ITERATIONS 1000000

int counter=0;
pthread_mutex_t lock;

void * increment_counter(void *arg)
{
	for(int i=1;i<NUM_ITERATIONS;i++)
	{
		pthread_mutex_lock(&lock);
		counter++;
		pthread_mutex_unlock(&lock);
	}
	return NULL;
}

int main()
{
	pthread_t t1,t2;

	pthread_mutex_init(&lock,NULL);

	pthread_create(&t1,NULL,increment_counter,NULL);
	pthread_create(&t2,NULL,increment_counter,NULL);

	pthread_join(t1,NULL);
	pthread_join(t2,NULL);

	pthread_mutex_destroy(&lock);

	printf("Value after counter incrementation:%d\n",counter);
	return 0;
}

