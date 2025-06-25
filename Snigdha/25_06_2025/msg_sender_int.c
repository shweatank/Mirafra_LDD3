#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#define MSG_KEY 5678
#define MSG_SIZE 100

// Message structure
struct msg_buffer 
{
    long msg_type;
    int a;
    int b;
    int result;
};

int main() 
{
    int msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) 
    {
        perror("msgget failed");
        return 1;
    }

    struct msg_buffer message;
    printf("Enter two integers:");
    scanf("%d%d",&message.a,&message.b);
    message.msg_type = 1;

    // Send message
    msgsnd(msgid, &message, sizeof(struct msg_buffer) - sizeof(long), 0);
    printf("Sender: Message sent a:%d b:%d.\n",message.a,message.b);

    msgrcv(msgid,&message,sizeof(struct msg_buffer)- sizeof(long),2,0);
    
    printf("Sender: Received result=%d from receiver\n",message.result);

    msgctl(msgid,IPC_RMID,NULL);
    return 0;
}
