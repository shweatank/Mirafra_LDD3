#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
int counter=0;
void *inc(void *arg)
{
	for(int i=0;i<10000;i++)
	{
		counter++;
	}
	return NULL;
}
int main()
{
	pthread_t t1,t2;
	pthread_create(&t1,NULL,inc,NULL);
	pthread_create(&t2,NULL,inc,NULL);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
}
// Address 0x10c014 is 0 bytes inside data symbol "counter"(multiple threads are access the same value);
//Locks held: none(no locks held)


