#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<wait.h>
#include<string.h>
#define size 1024
int main()
{
	key_t key=ftok("shared memory",65);
	int shmid=shmget(key,size, 0664| IPC_CREAT);
	if(shmid==-1)
	{
		perror("shmid");
		return EXIT_FAILURE;
	}
	pid_t pid;
        pid=fork();     
      	if(pid==-1)
	{
		perror("fork");
		return EXIT_FAILURE;
	}
	else if(pid==0)
	{
		sleep(1);
		char *ptr=(char *)(shmat(shmid,NULL,0));
		if(ptr==(char *)-1)
		{
			perror("child shmat");
			exit(1);
		}
		printf("child read from shared memory: %s\n",ptr);
		shmdt(ptr);
		exit(0);
	}	
	else
	{
		char *ptr=(char *)(shmat(shmid,NULL,0));
		if(ptr==(char *)-1)
		{
			perror("parent shmat");
			exit(1);
		}
		strcpy(ptr,"swarna");
		printf("data written to shared memory\n");
		shmdt(ptr);
		wait(NULL);
		shmctl(shmid,IPC_RMID,NULL);
		printf("parent removed shared memory\n");
	}
}
