#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *thread_fun(void *arg)
{
	int val=*((int *)arg);
	printf("Thread ID:%lu\n",pthread_self());
	printf("Received argument:%d\n",val);
	pthread_exit(NULL);
}


int main()
{
	pthread_t tid;
	int value=42;

	if(pthread_create(&tid,NULL,thread_fun,&value) != 0)
	{
		perror("Failed to create thread\n");
		return 1;
	}
	pthread_join(tid,NULL);
	return 0;
}
