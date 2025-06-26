#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MSG_KEY 5678
#define MSG_SIZE 100

// Message structure
struct msg_buffer {
    long msg_type;
    int data[2];
};

int main() {
    int msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget failed");
        return 1;
    }

    struct msg_buffer message;
    message.msg_type = 1;
    printf("Enter the data : ");
    scanf("%d%d",&message.data[0],&message.data[1]);
    // Send message
    msgsnd(msgid, &message, sizeof(int)*2, 0);
    printf("Sender: Message sent.\n");

    return 0;
}
