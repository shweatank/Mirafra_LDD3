#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
pthread_spinlock_t spinlock;
void *thread_function(void *arg)
{
        int thread_num=*(int*)arg;
        printf("thread %d trying to acquier spinlock----\n",thread_num);
        pthread_spin_lock(&spinlock);
        printf("thread %d acquiredthe spinlock--\n",thread_num);
        sleep(2);
        printf("thread %d relesing the thread lock---\n",thread_num);
        pthread_spin_unlock(&spinlock);
i       return NULL;
}
int main()
{
        pthread_t t1,t2;
        int id1=1,id2=2;
        pthread_spin_init(&spinlock,0);//initialise the spin lock
        pthread_create(&t1,NULL,thread_function,&id1);
        sleep(1);//ensures thread 1 gets spinlock first
         pthread_create(&t2,NULL,thread_function,&id2);
       pthread_join(t1,NULL);
       pthread_join(t2,NULL);
       pthread_spin_destroy(&spinlock);

}
/*thread 1 trying to acquier spinlock----
thread 1 acquiredthe spinlock--
thread 2 trying to acquier spinlock----
thread 1 relesing the thread lock---
thread 2 acquiredthe spinlock--
thread 2 relesing the thread lock---*/

