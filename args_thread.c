#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
struct data
{
   int roll;
   char name[20];
};
void *myfunction(void *arg)
{
  struct data *s=(struct data*)arg;
   printf("thread is running\n");
   printf("%d %s\n",s->roll,s->name);
}
int main()
{
   pthread_t t;
   struct data *d=malloc(sizeof(struct data));
   d->roll=21;
   strcpy(d->name,"kavya");
   pthread_create(&t,NULL,myfunction,d);
   //sleep(1);
   pthread_join(t,NULL);//it is neccessary to  wait untill the thread completes its execution without this the thread doesnot get enough time to execute thread then the main thread blocks
}
