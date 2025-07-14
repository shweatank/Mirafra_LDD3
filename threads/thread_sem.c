#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>

#define NUM_ITERATIONS 1000000

int counter=0;
sem_t sem;

void * increment_counter(void *arg)
{
	for(int i=1;i<NUM_ITERATIONS;i++)
	{
		sem_wait(&sem);
		counter++;
		sem_post(&sem);
	}
	return NULL;
}

int main()
{
	pthread_t t1,t2;

	sem_init(&sem,0,1);

	pthread_create(&t1,NULL,increment_counter,NULL);
	pthread_create(&t2,NULL,increment_counter,NULL);

	pthread_join(t1,NULL);
	pthread_join(t2,NULL);

	sem_destroy(&sem);

	printf("Value after counter incrementation:%d\n",counter);
	return 0;
}

