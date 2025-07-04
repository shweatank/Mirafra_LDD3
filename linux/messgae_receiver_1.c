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

    // Receive request
    msgrcv(msgid, &message, sizeof(message.mtext), 1, 0);
    printf("Server: Request received: %s\n", message.mtext);

    // Send response
    message.mtype = 2;
    strcpy(message.mtext, "Acknowledged by server!");
    msgsnd(msgid, &message, sizeof(message.mtext), 0);
    printf("Server: Response sent.\n");

    return 0;
}

