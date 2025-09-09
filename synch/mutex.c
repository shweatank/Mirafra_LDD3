#include<pthread.h>
int counter=0;//shared resource
//pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex;
//function to inc the counter
void *increment (void *arg)
{
        //lock  the mutex toaccess the resource
        pthread_mutex_lock(&mutex);
        int temp=counter;
        temp=++temp;
        usleep(100000);
        counter=temp;
        printf("counter is now %d\n",counter);
        //unlock the mutex
        pthread_mutex_unlock(&mutex);
        return NULL;
}
int main()
{
        pthread_t threads[5];
        pthread_mutex_init(&mutex,NULL);
        //creating the multiple threads
        for(int i=0;i<5;i++)
        {
                pthread_create(&threads[i],NULL,increment,NULL);
        }
         for(int i=0;i<5;i++)
        {
                pthread_join(threads[i],NULL);
        }
//this might print before threads finish
        printf("final counter:%d\n",counter);
}

//=PTHREAD_MUTEX_INITIALIZER is a macro provided by the posix threads library.it is used to intialize the mutex to default state
//this intializer ensures that the mutex is set up without needing to call pthread_mutex_init explicitly
