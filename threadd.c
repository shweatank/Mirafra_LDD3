#include<stdio.h>
#include<pthread.h>
int count;
void *thread_1(void *p)
{
	for(int i=0;i<100;i++)
	{
		count++;
	}
}
int main()
{
	pthread_t t1;
	pthread_create(&t1,0,thread_1,0);
	for(int i=0;i<100;i++)
	{
		count++;
	}
	pthread_join(t1);
}















































