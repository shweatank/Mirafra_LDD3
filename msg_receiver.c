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
    struct msgbuf msg_recv;
    struct msgbuf msg_send;
    
    msgid = msgget(key, 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }
    
    if (msgrcv(msgid, &msg_recv, sizeof(struct msgbuf), 1, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }

    int sum = msg_recv.num1 + msg_recv.num2;
    printf("Received numbers: %d, %d. Sending sum: %d\n", msg_recv.num1, msg_recv.num2, sum);

    // Send result back to sender (type 2)
    msg_send.mtype = 2;
    msg_send.num1 = sum;
    if (msgsnd(msgid, &msg_send, sizeof(struct msgbuf), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    return 0;
}
