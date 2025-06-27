//process 2 returns result to process 1 
#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>
#include<pthread.h>
#include<signal.h>

typedef struct structure
{
	int num1;
	int num2;
	int result;
	int flag;
}packet;
void start(int sig)
{
	printf("Powered ON...\n");
	
}
packet *pack;
void read_data(int sig)
{
	printf("Enter the numbers : ");
	scanf("%d%d",&pack->num1,&pack->num2);
	pack->flag=1;
}
void read_result(int sig)
{
	printf("Result = %d\n",pack->result);
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
			return;
		}
		pack->flag=0;
	int r;
	if(r=fork()){
	signal(SIGINT,read_data);
	signal(SIGQUIT,SIG_IGN);
		printf("Read data process... : %d\n",getpid());
		printf("Result process  : %d\n",r);
		while(1);
		shmdt(pack);
	}
	else
	{
	signal(SIGINT,SIG_IGN);	
	signal(SIGQUIT,read_result);
		while(1);
		shmdt(pack);
	}
}
