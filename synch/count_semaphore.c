#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
sem_t count_semaphore;
int ava_slots=3;//pool of 3 resourcs
void * use_resource(void *arg)
{
        sem_wait(&count_semaphore);
        printf("thread %ld:usinga resource .available slots:%d\n",(long)arg,ava_slots-1);
        ava_slots--;
        sleep(1);
        ava_slots++;
        sem_post(&count_semaphore);
        return NULL;
}
int main()
{
        pthread_t threads[5];
        int num_threads=5;
        sem_init(&count_semaphore,0,3);
        for(long i=0;i<num_threads;i++)
        {
                pthread_create(&threads[i],NULL,use_resource,(void *)i);
        }
         for(long i=0;i<num_threads;i++)
        {
                pthread_join(threads[i],NULL);
        }
         return 0;
}
