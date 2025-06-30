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

// Message structure
struct msg_buffer 
{
    long msg_type;
    int msg_data[DATA_SIZE];
};

int main() 
{
    int msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) 
    {
        perror("msgget failed");
        return 1;
    }

    sem_t *semaphore = sem_open(SEMAPHORE, O_CREAT, 0666, 0);
    
    if (semaphore == SEM_FAILED) 
    {
        perror("sem_open failed");
        return 1;
    }

    struct msg_buffer message;
    message.msg_type = 1;
    message.msg_data[0]  = 5;
    message.msg_data[1]  = 6;

    sem_post(semaphore);
    // Send message
    msgsnd(msgid, &message, sizeof(message.msg_data), 0);
  
    struct msg_buffer messageR;
    messageR.msg_type = 2;

    msgrcv(msgid, &messageR, sizeof(messageR.msg_data), 2, 0);

    printf("Final result = %d\n", messageR.msg_data[2]);

    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}
