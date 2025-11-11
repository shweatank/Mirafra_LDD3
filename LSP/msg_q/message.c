#include<stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
typedef struct m
{
	long int type;
	char data[100];

}message;
int main()
{
	key_t key=ftok("message",65);
	int msgid=msgget(key,0666|IPC_CREAT);
	message msg;
	msg.type=1;
	strcpy(msg.data,"kusuma");
	if(msgsnd(msgid,&msg,sizeof(msg.data),0)==-1)
	{
		perror("msgsend failed\n");
		return EXIT_FAILURE;
	}
	printf("message send:%s\n",msg.data);
	if(msgrcv(msgid,&msg,sizeof(msg.data),1,0)==-1)
	{
		perror("msgrecv failed\n");
		return EXIT_FAILURE;
	}

	printf("message receive:%s\n",msg.data);
if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl failed");
        exit(EXIT_FAILURE);
    }

    printf("Message queue removed.\n");

    return 0;
}
