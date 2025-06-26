#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MSG_KEY 234
#define MSG_SIZE 10

struct msg_buffer
{
	long msg_type;
	int msg_text[MSG_SIZE];
};

int main()
{
	int a,b;
	int msgid = msgget(MSG_KEY,0666 | IPC_CREAT);
	if(msgid == -1)
	{
		perror("mesg get ERROR");
		return 1;
	}

	struct msg_buffer message;

	message.msg_type =1;
	int pid;

	if((pid=fork())<0)
	{
		perror("FORK FAILED!");
		return 1;
	}
	else if(pid == 0)
	{
	 //CHILD PROCESS
	 printf("Enter the Two numbers:\n");
	 scanf("%d %d",&a,&b);

	 message.msg_text[0] = a;
	 message.msg_text[1] = b;

	 msgsnd(msgid,&message,sizeof(message.msg_text),0);
	 printf("VALUES ARE SENT \n");
	}
	else
	{
		//PARENT PROCESS

		waitpid(pid,NULL,0);
		sleep(10);

		printf("RECEIVER !\n");
		msgrcv(msgid,&message,sizeof(message.msg_text),1,0);

		printf("Received :%d\n",message.msg_text[0]);
		printf("Removing QUEUE.....\n");

		msgctl(msgid,IPC_RMID,NULL);
		return 0;
	}

}
