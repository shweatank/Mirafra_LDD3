// p2.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024
#define MSG_KEY 123
#define MSG_SIZE 2

struct msg_buffer {
    long msg_type;
    int msg_text[MSG_SIZE];
};

typedef struct {
    int a;
    int b;
    int op;
} Args;

void *perform_operation(void *arg) {
    Args args = *(Args *)arg;
    int result;

    switch (args.op) {
        case 0: result = args.a + args.b; break;
        case 1: result = args.a - args.b; break;
        case 2: result = args.a * args.b; break;
        case 3:
            result = (args.b == 0) ? 0 : args.a / args.b;
            break;
        default: result = 0;
    }

    struct msg_buffer message;
    message.msg_type = 1;
    message.msg_text[0] = args.op;
    message.msg_text[1] = result;

    int msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    msgsnd(msgid, &message, sizeof(message.msg_text), 0);

    const char *ops[] = {"Addition", "Subtraction", "Multiplication", "Division"};
    printf("%s result computed: %d\n", ops[args.op], result);

    pthread_exit(NULL);
}

int main() {
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    int *data = (int *)shmat(shmid, NULL, 0);
    if (data == (int *)-1) {
        perror("shmat");
        exit(1);
    }

    data[0] = -1; // ready to receive

    printf("Process 2 running (PID %d). Waiting for data...\n", getpid());

    while (1) {
        if (data[0] >= 0 && data[0] <= 3) {
            Args args;
            args.op = data[0];
            args.a = data[1];
            args.b = data[2];

            pthread_t tid;
            pthread_create(&tid, NULL, perform_operation, &args);
            pthread_join(tid, NULL);

            data[0] = -1; // mark as processed
        } else {
            usleep(100000);
        }
    }

    return 0;
}

