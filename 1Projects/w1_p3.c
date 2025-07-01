// p3.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#define MSG_KEY 123
#define MSG_SIZE 2

struct msg_buffer {
    long msg_type;
    int msg_text[MSG_SIZE];
};

int msgid;

void handle_exit(int sig) {
    msgctl(msgid, IPC_RMID, NULL); // Remove message queue
    printf("\nMessage queue removed. Exiting Process 3.\n");
    exit(0);
}

int main() {
    signal(SIGINT, handle_exit); // Allow Ctrl+C to cleanup

    msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    printf("Process 3 running (PID %d). Waiting for messages...\n", getpid());

    struct msg_buffer message;

    while (1) {
        if (msgrcv(msgid, &message, sizeof(message.msg_text), 1, 0) != -1) {
            int op = message.msg_text[0];
            int result = message.msg_text[1];

            const char *ops[] = {"Addition", "Subtraction", "Multiplication", "Division"};
            const char *type = (result % 2 == 0) ? "Even" : "Odd";

            printf("%s result received: %d — It is %s.\n", ops[op], result, type);
        } else {
            perror("msgrcv");
        }
    }

    return 0;
}

