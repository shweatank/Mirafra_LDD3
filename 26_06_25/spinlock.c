#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#define NUM 10000
int counter=0;
pthread_spinlock_t spinlock;
void *inc(void *arg)
{
	for(int i=0;i<NUM;i++)
	{
		pthread_spin_lock(&spinlock);
		counter++;
		pthread_spin_unlock(&spinlock);
	}
	return NULL;
}
int main()
{
	pthread_t t1,t2;
	pthread_spin_init(&spinlock,0);
	pthread_create(&t1,NULL,inc,NULL);
	pthread_create(&t2,NULL,inc,NULL);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	printf("%d\n",counter);
	return 0;
}
