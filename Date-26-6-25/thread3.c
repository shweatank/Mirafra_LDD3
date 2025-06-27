#include<stdio.h>
#include<pthread.h>
#include <semaphore.h>
int c=0;
sem_t sem;
void *thread1(void *p)
{
	printf("The Thread id is %ld and messege is %s\n",pthread_self(),(char *)p);
	printf("c=%d\n",c);
	sem_wait(&sem);
	c++;
	sem_post(&sem);
	printf("c=%d\n",c);
	return 0;
}
void *thread2(void *p)
{
	printf("The Thread id is %ld and messege is %s\n",pthread_self(),(char *)p);
	printf("c=%d\n",c);
	sem_wait(&sem);
	c--;
	sem_post(&sem);
	printf("c=%d\n",c);
	return 0;
}
int main()
{
	pthread_t t1,t2;
	char a[20]="hello";
	char b[20]="bye";
	sem_init(&sem,0,1);
	pthread_create(&t1,0,thread1,(char *)a);
	pthread_create(&t2,0,thread2,(char *)b);

	pthread_join(t1,0);
	pthread_join(t2,0);
}


