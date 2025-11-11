#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#define MAX 10
int cnt=0;
pthread_t t1,t2;
pthread_mutex_t m;
pthread_cond_t odd,even;
void *even_fun(void *arg)
{
	while(cnt<=MAX)
	{
		pthread_mutex_lock(&m);
		if(cnt%2==0)
		{
			printf("%d ",cnt);
			cnt++;
			pthread_cond_signal(&odd);
		}
		else
			pthread_cond_wait(&even,&m);
		pthread_mutex_unlock(&m);
	}
}

void *odd_fun(void *arg)
{
	while(cnt<=MAX)
	{
		pthread_mutex_lock(&m);
		if(cnt%2!=0)
		{
			printf("%d ",cnt);
			cnt++;
			pthread_cond_signal(&even);
		}
		else
			pthread_cond_wait(&odd,&m);
		pthread_mutex_unlock(&m);
	}
}
int main()
{
	pthread_mutex_init(&m,NULL);
	pthread_cond_init(&even,NULL);
	pthread_cond_init(&odd,NULL);
	pthread_create(&t1,NULL,even_fun,NULL);
	pthread_create(&t2,NULL,odd_fun,NULL);

	pthread_join(t1,NULL);
	pthread_join(t2,NULL);

	printf("Two threads are completed\n");
}
