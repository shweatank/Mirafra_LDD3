#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>

int num=40;
sem_t sem;

void * increment_counter(void *arg)
{
	sem_wait(&sem);
	printf("In thread 1\n");
	for(int i=0;i<10;i++)
	{
		printf("%d ",num++);
		sleep(1);
	}
	printf("\n");
	sem_post(&sem);

	return NULL;
}

void * decrement_counter(void *arg)
{
        sem_wait(&sem);
	printf("In thread 2\n");
        for(int i=0;i<10;i++)
        {
                printf("%d ",num--);
		sleep(1);
        }
        sem_post(&sem);

        return NULL;
}


int main()
{
	pthread_t t1,t2;

	sem_init(&sem,0,1);

	pthread_create(&t1,NULL,increment_counter,NULL);
	pthread_create(&t2,NULL,decrement_counter,NULL);

	pthread_join(t1,NULL);
	pthread_join(t2,NULL);

	sem_destroy(&sem);

	return 0;
}

