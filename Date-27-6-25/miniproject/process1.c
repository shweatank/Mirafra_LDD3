#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#define SHM_KEY 1234
#define SHM_SIZE 1024
typedef struct 
{
	int data1;
	int data2;
	int result;
	int flag;
}data;
int shmid;
data *shared_data;
void signal_handler(int num)
{
	printf("The signal is caught is %d\n",num);
	if(num==2)
	{	
		printf("Enter The Data To Shared memory\n");
		scanf("%d %d",&shared_data->data1,&shared_data->data2);
		printf("Wrote to shared memory.\n");
		shared_data->flag=1;
		printf("Waiting for Signal..\n");
	}
	else if(num==3)
	{
		printf("Reading from to shared memory..\nThe final result is : %d\n",shared_data->result);
		shmdt(shared_data);
                shmctl(shmid, IPC_RMID, NULL);
		exit(0);
	}
}
int main()
{
	shmid = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
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
	shared_data->flag=0;
	if(fork()==0)
	{
		signal(3,signal_handler);
		printf("The pid of child process : %d\n",getpid());
	}
	else
	{	
		signal(2,signal_handler);
		printf("The pid of parent process : %d\n",getpid());
	}
	while(1) pause();
}
