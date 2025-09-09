#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
sem_t semaphore;
int count=0;
void *inc(void *arg)
{
        sem_wait(&semaphore);
        printf("locking the semaphore:thread 1\n");
        int temp=count;
        temp=temp+5;
        count=temp;
        sleep(5);
//      sem_post(&semaphore);
        return NULL;
}
void *inc1(void *arg)
{
        sem_post(&semaphore);
        printf("unlocking the semaphore:thread1\n");
        sem_wait(&semaphore);
        printf("locking the semaphore :thread 2\n");
        int temp=count;
        temp=temp+1000;
        //sleep(1);
        count=temp;
        printf("unlocking the semaphore:thread 2\n");
        sem_post(&semaphore);
        return NULL;
}
int main()
{
        pthread_t t1,t2;
        sem_init(&semaphore,0,1);
        pthread_create(&t1,NULL,inc,NULL);
        pthread_create(&t2,NULL,inc1,NULL);
        pthread_join(t1,NULL);
        pthread_join(t2,NULL);
        printf("%d\n",count);
        sem_destroy(&semaphore); 
}
