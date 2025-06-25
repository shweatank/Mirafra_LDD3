#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024
int main()
{
	int shmid=shmget(SHM_KEY,SHM_SIZE,0666|IPC_CREAT);
	if(shmid==-1){
		perror("shmget failed");
		return 1;
	}

	char *data=(char *)shmat(shmid,NULL,0);
	if(data==(char *)-1)
	{
		perror("shmat failed");
		return 1;
	}
	strcpy(data,"Hello world\n");
	printf("Writer:wrote\n");
	printf("Reader:%s\n",data);

	shmdt(data);
}




