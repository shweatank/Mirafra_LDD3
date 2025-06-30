#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/shm.h>
#include<signal.h>
#define KEY 123
#define SHM_SIZE 1024
struct node
{
	int a;
	int b;
	int result;
}*s=NULL;
int count=0;
void sighandler(int signo)
{
	count++;
	switch(count)
	{
		case 1: s->result=s->a+s->b;
			printf("%d\n",s->result);
			break;
		case 2: s->result=s->a-s->b;
			printf("%d\n",s->result);
			break;
		case 3: s->result=s->a*s->b;
			printf("%d\n",s->result);
			break;
		case 4: if(s->b!=0)
			{
				s->result=s->a/s->b;
				printf("%d\n",s->result);
			}
			else
			{
				printf("zero division error\n");
			}
			exit(0);
		default: printf("All operations are done\n");
			 exit(0);
	}
}
void main()
{
	int shmid=shmget(KEY,SHM_SIZE,0);
	if(shmid==-1)
	{
		perror("shmget");
	}
	s=(struct node *)shmat(shmid,NULL,0);
	signal(SIGINT,sighandler);
	sigset_t block;
	sigemptyset(&block);
	sigaddset(&block,SIGINT);
	sigprocmask(SIG_BLOCK,&block,NULL);
	printf("Blockig the signal for 5 sec\n");
	sleep(5);
	printf("Unblocking the signal\n");
	sigprocmask(SIG_UNBLOCK,&block,NULL);
	while(1)
	{
		printf("Waiting for signal (%d/4)....\n",count+1);
		pause();
	}
}

