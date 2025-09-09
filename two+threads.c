#include<stdio.h>
#include<pthread.h>
#include<string.h>
struct data
{
  int min;
  int max;
};
void *printeven(void *arg)
{
   struct data *s=(struct data*)arg;
   for(int i=s->min;i<=s->max;i++)
   {
      if(i%2==0)
      {
        printf("%d\n",i);
      }
   }
}
void *printodd(void *arg)
{
   struct data *s=(struct data*)arg;
   for(int i=s->min;i<=s->max;i++)
   {
      if(i%2==1)
      {
        printf("%d\n",i);
      }
   }
}
int main()
{
    pthread_t t1,t2;
    struct data ptr;
    ptr.min=1;
    ptr.max=20;
    pthread_create(&t1,NULL,printeven,&ptr);
    pthread_create(&t2,NULL,printodd,&ptr);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
}
   
