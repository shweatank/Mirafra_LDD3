#include<stdio.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<string.h>
#include<sys/shm.h>
#define KEY 12345
#define SHM_SIZE 1024
int main()
{
	int shmid=shmget(KEY,SHM_SIZE,IPC_CREAT|0666);
	if(shmid==-1)
	{
		perror("shmget");
		return -1;
	}
	int *shm_ptr=(int *)shmat(shmid,NULL,0);
	int x=10,y=30;
	shm_ptr[0]=x;
	shm_ptr[1]=y;
	printf("Data sent\n");
	printf("Result is %d\n",shm_ptr[2]);
	return 0;
}
