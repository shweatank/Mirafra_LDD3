#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>
#include<pthread.h>
typedef struct structure
{
	int num1;
	int num2;
	int result;
	int flag;
}packet;
packet *pack;
int shmid;
pthread_mutex_t lock;
int result,flag=0;
void *thread1(void *arg)
{
	while(pack->flag!=1);
	pthread_mutex_lock(&lock);
	result=pack->num1+pack->num2;
	pack->flag=2;
	pthread_mutex_unlock(&lock);
	flag=1;
	return NULL;
}
void* thread2(void *arg)
{
	while(pack->flag!=2);
	pthread_mutex_lock(&lock);
	//printf("Entered thread2 CS...\n");
	pack->result=result;
	pack->flag=3;
	flag=0;
	pthread_mutex_unlock(&lock);
	return NULL;
}
void main()
{
	int shmid=shmget(123,50,IPC_CREAT|0664);
	if(shmid==-1)
	{
		perror("shmget");
		return;
	}
	pack=(packet *)shmat(shmid,NULL,0);
	if(pack==(void *)-1)
	{
		perror("shmat");
		return ;
	}
	pthread_t t1,t2;
	pthread_mutex_init(&lock,NULL);
	while(1){
	pthread_create(&t1,NULL,thread1,0);
	pthread_create(&t2,NULL,thread2,0);
	pthread_join(t1,0);
	pthread_join(t2,0);
	}
	pthread_mutex_destroy(&lock);
	shmdt(pack);
}

