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

    int shmid = shmget(key, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    char *str = (char *)shmat(shmid, (void *)0, 0);
    if (str == (char *)(-1)) {
        perror("shmat failed");
        exit(1);
    }

    while (1) {
        if (strlen(str) > 0) {
            if (strcmp(str, "exit") == 0)
                break;
            printf("P2 received: %s\n", str);
            str[0] = '\0';
        }
        usleep(100000);
    }

    shmdt(str);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}

