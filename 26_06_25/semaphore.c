#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#define NUM 10000
int counter=0;
sem_t sem;
void *inc(void *arg)
{
	for(int i=0;i<NUM;i++)
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
	pthread_create(&t1,NULL,inc,NULL);
	pthread_create(&t2,NULL,inc,NULL);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	printf("%d\n",counter);
	return 0;
}
