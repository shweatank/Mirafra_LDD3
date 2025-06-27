#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>

#define SHM_SIZE 1024

int main() {
    key_t key = ftok("shmfile", 65);
    if (key == -1) {
        perror("ftok failed");
        exit(1);
    }

    int shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    char *str = (char *)shmat(shmid, (void *)0, 0);
    if (str == (char *)(-1)) {
        perror("shmat failed");
        exit(1);
    }

    char input[SHM_SIZE];
    while (1) {
        printf("P1 send: ");
        fgets(input, SHM_SIZE, stdin);
        input[strcspn(input, "\n")] = '\0';
        strcpy(str, input);

        if (strcmp(input, "exit") == 0)
            break;

        while (strlen(str) > 0)
            usleep(100000);
    }

    shmdt(str);
    return 0;
}

