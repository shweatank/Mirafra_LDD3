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
	int i,j;
	for(i=0;i<20;i++)
		a[i]=i;
         printf("Writer: wrote\n");
	 for(i=0;i<20;i++)
		 printf("Reader:%d\n",a[j]);
	 shmdt(a);
}
