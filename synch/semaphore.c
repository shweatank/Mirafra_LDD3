#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
sem_t semaphore;//intially 1
int counter=0;
void *increment_counter(void *arg)
{
        sem_wait(&semaphore);//wait(dec semaphore)
        int temp=counter;
        temp=temp+1;
        counter=temp;
        sem_post(&semaphore);//work start ayaka 1-->0 chesthadi wait ,,work ayipoyaka 0---->1 chestadi post
        return NULL;
}
int main()
{
        pthread_t threads[2];
        sem_init(&semaphore,0,1);//the semaphore is intialized with 1,that is one thread can eter into the thread at a time.....
        pthread_create(&threads[0],NULL,increment_counter,NULL);
        pthread_create(&threads[1],NULL,increment_counter,NULL);
        pthread_join(threads[0],NULL);
        pthread_join(threads[1],NULL);
        printf("final val of counter:%d",counter);
}

~                                                  
