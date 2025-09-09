#include<stdio.h>
#include<pthread.h>
void *myfunction(void *arg)
{
   printf("thread is running\n");
   return NULL;
}
int main()
{
    pthread_t t;
    pthread_create(&t,NULL,myfunction,NULL);
    pthread_join(t,NULL);
}
