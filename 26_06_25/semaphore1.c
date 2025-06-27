#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
sem_t sem;
int counter=0;
void *fun(void *arg)
{
	sem_wait(&sem);
	printf("statement %d\n",++counter);
	sem_post(&sem);
	return NULL;
}
void main()
{
	pthread_t t1,t2,t3;
	sem_init(&sem,0,3);
	pthread_create(&t1,NULL,fun,NULL);
	pthread_create(&t2,NULL,fun,NULL);
	pthread_create(&t3,NULL,fun,NULL);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	pthread_join(t3,NULL);
}
