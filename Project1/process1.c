#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#define KEY 123
#define SHM_SIZE 1024
struct node
{
	int a;
	int b;
	int result;
};
void main()
{
	int shmid=shmget(KEY,SHM_SIZE,IPC_CREAT|0666);
	if(shmid==-1)
	{
		perror("shmget");
	}
struct node *s=(struct node *)shmat(shmid,NULL,0);
	printf("Enter a and b values ");
	scanf("%d %d",&s->a,&s->b);
	printf("Data written to shared memory\n");
}

