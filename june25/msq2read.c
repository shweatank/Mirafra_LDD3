#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_KEY 5678
#define MSG_SIZE 100

struct msg_buffer {
    long msg_type;
    int data[2];
};

int main() {
    int msgid = msgget(MSG_KEY, 0666);
    if (msgid == -1) {
        perror("msgget failed");
        return 1;
    }

    struct msg_buffer message;

    // Receive message
    msgrcv(msgid, &message, sizeof(int)*2, 1, 0);
    printf("Receiver: Message received: \n");
    int result=message.data[0]+message.data[1];
    //printf("Result=%d\n",result);
    message.msg_type=2;
    message.data[0]=result;
    msgsnd(msgid,&message,sizeof(int),0);
    // Remove the queue
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}
