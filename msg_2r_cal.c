#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
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
	int msgid = msgget(MSG_KEY,0666 | IPC_CREAT);

	struct msg_buffer message;

	message.msg_type =1;

	msgrcv(msgid,&message,sizeof(message.msg_text),1,0);
	
	printf("RECEVIED %d..%d\n",message.msg_text[0],message.msg_text[1]);
	int temp = message.msg_text[0]+message.msg_text[1];

	message.msg_text[0]=temp;
	msgsnd(msgid,&message,sizeof(message.msg_text),0);
	printf("SENT THE VALUE ADDITION\n");

}

