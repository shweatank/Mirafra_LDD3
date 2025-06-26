#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MSG_KEY 5678
#define MSG_SIZE 100


struct msg_buffer {
    long msg_type;
    char msg_text[MSG_SIZE];
};

int main() {
    int msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget failed");
        return 1;
    }

    struct msg_buffer message;
    message.msg_type = 1;
    int a=10,b=20;
    snprintf(message.msg_text,sizeof(message.msg_text),"%d %d",a,b);
    msgsnd(msgid, &message, sizeof(message.msg_text), 0);
    printf("Sender: Message sent.\n");

    msgrcv(msgid,&message,sizeof(message.msg_text),2,0);
    int result=atoi(message.msg_text);
    printf("The Result is %d\n",result);
    msgctl(msgid, IPC_RMID, NULL); 

    return 0;
}
