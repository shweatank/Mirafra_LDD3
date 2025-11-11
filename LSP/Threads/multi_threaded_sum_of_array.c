#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#define size 10
int arr[size]={1,2,3,4,5,6,7,8,9,10};
int sum=0;
pthread_t t1,t2;
pthread_mutex_t m1;
void *partial_sum(void *arg)
{
	int i,j,sum1=0;
	int *range=(int *)arg;
	i=range[0];
	j=range[1];
	for(int a=i;a<j;a++)
		sum1+=arr[a];
	pthread_mutex_lock(&m1);
	sum+=sum1;
	pthread_mutex_unlock(&m1);
}
int main()
{
	int range1[2]={0,size/2};
	int range2[2]={size/2,size};
	pthread_create(&t1,NULL,partial_sum,range1);
	pthread_create(&t2,NULL,partial_sum,range2);

	pthread_join(t1,NULL);
	pthread_join(t2,NULL);

	printf("two threads completed and final sum:%d\n",sum);
}


