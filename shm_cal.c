#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include<unistd.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024

int main() {
    // Create shared memory segment
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }

    // Attach to shared memory
    int *data = (int *)shmat(shmid, NULL, 0);
    if (data == (int *)-1) {
        perror("shmat failed");
        return 1;
    }
	data[0]=5;
	data[1]=3;

    // Write data

    printf("Writer: Wrote to shared memory.\n");
    sleep(10);
    printf("result=%d \n",data[2]);

    // Detach
    shmdt(data);
    return 0;
}

