#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024

int main() {
    // Get existing shared memory segment
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }

    // Attach
    int *data = (int *)shmat(shmid, NULL, 0);
    if (data == (int *)-1) {
        perror("shmat failed");
        return 1;
    }

    printf("data[0]=%d data[1]=%d\n", data[0],data[1]);
    data[2]=data[0]+data[1];
    printf("result =%d \n",data[2]);

    // Detach and optionally remove
    shmdt(data);
    //shmctl(shmid, IPC_RMID, NULL);  // remove shared memory
    return 0;
}
