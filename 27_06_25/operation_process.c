#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>
#include<pthread.h>

#define SHM_KEY  1234
#define SHM_SIZE 1024
int result;

struct info
{
	int a;
	int b;
	int result;
	int flag;
};

struct info *shared_data;
int shmid;
pthread_mutex_t lock;

void *thread1(void *arg)
{
	while(shared_data->flag!=1);
	pthread_mutex_lock(&lock);//locks the shm with mutex
	result=shared_data->a+shared_data->b;
	shared_data->flag=2;
	pthread_mutex_unlock(&lock);//unlocks the shm
	return NULL;
}
void *thread2(void *arg)
{
	while(shared_data->flag!=2);
	pthread_mutex_lock(&lock);//locks the shm with mutex
        shared_data->result=result;

        pthread_mutex_unlock(&lock);//unlocks the shm
        return NULL;
}
int main()
{
	 // Create shared memory segment
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }

    // Attach to shared memory
    shared_data = (struct info*)shmat(shmid, NULL, 0);
     if (shared_data == (void *)-1) {
        perror("shmat failed");
        return 1;
    }

    pthread_t t1,t2;
    pthread_mutex_init(&lock,NULL);
    while(1)
    {
	    pthread_create(&t1,NULL,thread1,0);
	    pthread_create(&t2,NULL,thread2,0);
	    pthread_join(t1,0);
	    pthread_join(t2,0);
    }
    pthread_mutex_destroy(&lock);
    shmdt(shared_data);
}







