#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<pthread.h>
#define SHM_KEY 1234
#define SHM_SIZE 1024
typedef struct 
{
	int data1;
	int data2;
	int result;
	int flag;
}data;

data *shared_data;
pthread_mutex_t lock;
int result;
void *thread1(void *p)
{
	pthread_mutex_lock(&lock);
	printf("Reading Shared memory contains: %d %d\n",shared_data->data1,shared_data->data2);
	result=shared_data->data1+shared_data->data2;
	printf("The Opertion is Done..\n");
	pthread_mutex_unlock(&lock);
	return NULL;
}
void *thread2(void *p)
{
	pthread_mutex_lock(&lock);
	shared_data->result=result;
	printf("writing Shared memory is done \n");
	pthread_mutex_unlock(&lock);
	return NULL;
}
int main()
{
        pthread_t t1,t2;
        pthread_mutex_init(&lock,NULL);
	
	int shmid = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
        if (shmid < 0)
       	{
		perror("shmget");
        	exit(1);
        }
	shared_data = (data *)shmat(shmid, NULL, 0);
	if (shared_data == (void *)-1) 
	{
        	perror("shmat");
        	exit(1);
    	}
	while(shared_data->flag!=1)
	{
		sleep(2);
		printf("Waiting for process-1 to write data..\n");
	}
	printf("The process-1 is writing is done..\n");
	pthread_create(&t1,0,thread1,0);
        pthread_join(t1,0);
        pthread_create(&t2,0,thread2,0);
        pthread_join(t2,0);
}

