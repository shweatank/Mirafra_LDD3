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
	int num1;//operand1
	int num2;//operand2
	int result;//stores result
	int flag;//monitors data is written/addition is done/result is sent back
}packet;
packet *pack;
void read_data(int sig)
{
	printf("Enter the numbers : ");
	scanf("%d%d",&pack->num1,&pack->num2);//write 2 numbers into shared memory
	pack->flag=1;//indicates data is written to shm
}
void read_result(int sig)
{
	printf("Result = %d\n",pack->result);//reads the result from shm and prints
}
void main()
{
	int shmid=shmget(123,50,IPC_CREAT|0664);//creates shm
	if(shmid==-1)/*shmget() failed*/
	{
		perror("shmget");
		return;
	}
		pack=(packet *)shmat(shmid,NULL,0);/*attaching shm to process*/
		if(pack==(void *)-1)/*shmat() failed*/
		{
			perror("shmat");
			return;
		}
		pack->flag=0;
	int r;
	if(r=fork()){//parent process which reads the inputs 
	signal(SIGINT,read_data);
	signal(SIGQUIT,SIG_IGN);
		printf("Read data process... : %d\n",getpid());
		printf("Result process  : %d\n",r);
		while(1);
		shmdt(pack);
	}
	else
	{//child process which prints the result
	signal(SIGINT,SIG_IGN);	
	signal(SIGQUIT,read_result);
		while(1);
		shmdt(pack);
	}
}
