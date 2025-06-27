#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
int c=0;
sem_t sem;
void *thread(void *p)
{
	printf("c=%d\n",c);
	sem_wait(&sem);
	c++;
	sem_post(&sem);
	printf("c=%d\n",c);
	return 0;
}

int main()
{
	sem_init(&sem,0,1);
	pthread_t t1,t2,t3;
	pthread_create(&t1,0,thread,0);
	pthread_create(&t2,0,thread,0);
	pthread_create(&t3,0,thread,0);

	pthread_join(t1,0);
	pthread_join(t2,0);
	pthread_join(t3,0);
}


