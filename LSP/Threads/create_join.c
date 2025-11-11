#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
pthread_t t1,t2;
void *thread1_fun(void *arg)
{
	int val=*(int *)arg;
	printf("thread1 recieved value:%d\n",val);
}
void *thread2_fun(void *arg)
{
	int val=*(int *)arg;
	printf("thread2 recieved value:%d\n",val);
}
int main()
{
	int val1=10,val2=20;
	pthread_create(&t1,NULL,thread1_fun,&val1);
	pthread_create(&t2,NULL,thread2_fun,&val2);
	
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	printf("two threads completed\n");
}
