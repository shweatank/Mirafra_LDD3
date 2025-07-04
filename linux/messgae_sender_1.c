#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

struct msgbuf {
    long mtype;
    char mtext[100];
};

int main() {
    key_t key = ftok("progfile", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);

    struct msgbuf message;
    message.mtype = 1;
    strcpy(message.mtext, "Hello from client!");
    msgsnd(msgid, &message, sizeof(message.mtext), 0);
    printf("Client: Request sent.\n");
    msgrcv(msgid, &message, sizeof(message.mtext), 2, 0);
    printf("Client: Response received: %s\n", message.mtext);

    return 0;
}

