#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MSG_KEY 5678
#define MSG_SIZE 100

// Message structure
struct msg_buffer {
    long msg_type;
    int a;
    int b;
    int result;
};

int main() {
	int ret;
    int msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget failed");
        return 1;
    }
    ret=fork();
    if(ret==-1)
    {
	    perror("message");
	    exit(0);
    }
    else if(ret==0)
    {
	struct msg_buffer message;
        msgrcv(msgid, &message, sizeof(message) - sizeof(long), 1, 0);
	message.result=message.a+message.b;
	printf("updating the value\n");
	sleep(1);
	printf("%d",message.result);
	message.msg_type=2;
	msgsnd(msgid,&message,sizeof(message)-sizeof(long),0);
    }
    else
    {
    struct msg_buffer message;
    message.msg_type = 1;
    message.a=2;
    message.b=34;
    // Send message
    msgsnd(msgid, &message, sizeof(message)-sizeof(long), 0);
    printf("Sender: Message sent.\n");
    wait(NULL);
    msgrcv(msgid,&message,sizeof(message)-sizeof(long),2,0);
    printf("final result:%d\n",message.result);
    msgctl(msgid,IPC_RMID,NULL);
    return 0;
    }
}

