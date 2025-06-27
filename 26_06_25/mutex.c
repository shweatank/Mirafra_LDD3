#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#define NUM 10000
int counter=0;
pthread_mutex_t lock;
void *inc(void *arg)
{
	for(int i=0;i<NUM;i++)
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
	pthread_create(&t1,NULL,inc,NULL);
	pthread_create(&t2,NULL,inc,NULL);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	printf("%d\n",counter);
	return 0;
}
