#include<stdio.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<stdlib.h>
#include<sys/shm.h>
#define KEY 12
#define SHM_SIZE 1024
struct var
{
	int a;
	int b;
	int add;
	int flag;
};
int main()
{
	int shmid=shmget(KEY,SHM_SIZE,IPC_CREAT|0666);
	if(shmid==-1)
	{
		perror("shmget");
	}
	struct var *s=(struct var *)shmat(shmid,NULL,0);
	for(int i=0;i<20;i++)
	{
		s->a=i;
		s->b=i;
		printf("Data received is: %d\n",s->add);
	}
	return 0;
}

