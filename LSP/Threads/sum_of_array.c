#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#define size 10
pthread_t t1,t2;
int arr[size]={1,2,3,4,5,6,7,8,9,10},sum1=0,sum2=0;
void *first_half(void *arg)
{
	for(int i=0;i<size/2;i++)
		sum1+=arr[i];
}
void *second_half(void *arg)
{
	for(int i=size/2;i<size;i++)
		sum2+=arr[i];
}
int main()
{
	pthread_create(&t1,NULL,first_half,NULL);
	pthread_create(&t2,NULL,second_half,NULL);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	int total=sum1+sum2;
	printf("Total:%d\n",total);
}
