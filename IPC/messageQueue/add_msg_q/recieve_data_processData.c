#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>


#define MSG_KEY 5678
#define DATA_SIZE 3
#define SEMAPHORE "/semForAddition"

struct msg_buffer {
    long msg_type;
    char msg_data[DATA_SIZE];
};

int main() {
    int msgid = msgget(MSG_KEY, 0666);
    if (msgid == -1) {
        perror("msgget failed");
        return 1;
    }

    sem_t *semaphore = sem_open(SEMAPHORE, 0);

    if (semaphore == SEM_FAILED)
    {
        perror("sem_open failed");
        return 1;
    }

    struct msg_buffer message;

    // Receive message
    msgrcv(msgid, &message, sizeof(message.msg_data), 1, 0);
   
    sem_wait(semaphore);

    message.msg_data[2] = message.msg_data[0] + message.msg_data[1];

    sem_post(semaphore);

    struct msg_buffer messageR;
    messageR.msg_type = 2;
    msgsnd(msgid, &message, sizeof(message.msg_data), 0);
    
    printf("Receiver: Message received: %s\n", message.msg_data);

    return 0;
}
