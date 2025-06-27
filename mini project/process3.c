#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>
#include<pthread.h>
typedef struct structure
{
	int num1;//operand1
	int num2;//operand2
	int result;//holds the result
	int flag;//monitors which thread works
}packet;
packet *pack;
int shmid;
pthread_mutex_t lock;
int result,flag=0;
void *thread1(void *arg)
{
	while(pack->flag!=1);
	pthread_mutex_lock(&lock);//locks the shm with mutex
	result=pack->num1+pack->num2;//calculates the sum of the 2 numbers
	pack->flag=2;
	pthread_mutex_unlock(&lock);//unlocks the shm
	return NULL;
}
void* thread2(void *arg)
{
	while(pack->flag!=2);
	pthread_mutex_lock(&lock);//locks the shm with mutex
	pack->result=result;//writing the result to shm
	pack->flag=3;
	pthread_mutex_unlock(&lock);//unlock the shm
	return NULL;
}
void main()
{
	int shmid=shmget(123,50,IPC_CREAT|0664);//creates shared memory
	if(shmid==-1)//shmget() failed
	{
		perror("shmget");
		return;
	}
	pack=(packet *)shmat(shmid,NULL,0);//attach shm with the process
	if(pack==(void *)-1)//shmat() failed
	{
		perror("shmat");
		return ;
	}
	pthread_t t1,t2;
	pthread_mutex_init(&lock,NULL);//initializing mutex
	while(1){
	pthread_create(&t1,NULL,thread1,0);//creating thread which calculates
	pthread_create(&t2,NULL,thread2,0);//creating thread which writes the result to shm
	pthread_join(t1,0);
	pthread_join(t2,0);
	}
	pthread_mutex_destroy(&lock);
	shmdt(pack);
}

