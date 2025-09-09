#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
pthread_spinlock_t spinlock;
void *thread1_function(void *arg1)
{
        int t1_num=*(int*)arg1;
        printf("thread %d is running--\n",t1_num);
        pthread_spin_lock(&spinlock);
        printf("thread %d is acquired the lock---\n",t1_num);
        sleep(3);
        printf("thread %d exiting--\n",t1_num);
        return NULL;
}
void *thread2_function(void *arg2)
{
        int t2_num=*(int*)arg2;
        sleep(1);
        printf("thtead %d is running---\n",t2_num);
        pthread_spin_unlock(&spinlock);
        printf("thread %d unlock spinlock---\n",t2_num);
        printf("thread %d exiting--\n",t2_num);
        return NULL;
}
int main()
{
        pthread_t t1,t2;
        int id1=1,id2=2;
        sleep(1);
        pthread_spin_init(&spinlock,0);
        pthread_create(&t1,NULL,thread1_function,&id1);
         pthread_create(&t2,NULL,thread2_function,&id2);
         pthread_join(t1,NULL);
         pthread_join(t2,NULL);
}
//unlike others spinlock do not involve blocking or putting the thread to sleep it just spins


