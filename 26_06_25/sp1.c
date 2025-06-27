#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024

int main()
{
	int shmid=shmget(SHM_KEY,SHM_SIZE,0666|IPC_CREAT);
	if(shmid==-1){
		perror("shmget failed");
		return 1;
	}
	int *a=(int *)shmat(shmid,NULL,0);
	if(a==(int *)-1)
	{
		perror("shmat failed");
		return 1;
	}
	a[0]=10;
	a[1]=20;
         a[2]=0;
         printf("Writer: wrote\n");
	 shmdt(a);
}
