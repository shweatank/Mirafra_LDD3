#include <stdio.h>
#include <pthread.h>
#include <string.h>

int counter = 0;
pthread_spinlock_t spin;

void* worker(void* arg)
{
	if(strcmp((char*)arg,"t1")==0)
	{
		pthread_spin_lock(&spin);
    		for(int i = 0;i<100000;i++)
    		{
        		counter++;
    		}
    		printf("in thread %s count %d \n",(char*)arg,counter);
    		pthread_spin_unlock(&spin);
	}
        if(strcmp((char*)arg,"t2")==0)
        {
		pthread_spin_lock(&spin);
    		for (int i = 0;i<100000;i++)
    		{
        		counter+=2;
    		}
    		printf("in thread %s count %d \n",(char*)arg,counter);
    		pthread_spin_unlock(&spin);
        }

    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_spin_init(&spin, PTHREAD_PROCESS_PRIVATE);

    pthread_create(&t1, NULL, worker, "t1");
    pthread_create(&t2, NULL, worker, "t2");

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_spin_destroy(&spin);

    printf("Final Counter Value: %d\n", counter);
    return 0;
}

