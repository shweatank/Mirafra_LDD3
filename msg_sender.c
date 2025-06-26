//send 2 numbers and print the addition result from 2nd process
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>

struct msgbuf {
    long mtype;
    int num1;
    int num2;
};

int main() {
    key_t key = 1234;
    int msgid;
    struct msgbuf msg_send = {1, 10, 20};
    struct msgbuf msg_recv;
    
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }
    if (msgsnd(msgid, &msg_send, sizeof(struct msgbuf), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    if (msgrcv(msgid, &msg_recv, sizeof(struct msgbuf), 2, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }

    printf("Received sum from receiver: %d\n", msg_recv.num1);
    return 0;
}
