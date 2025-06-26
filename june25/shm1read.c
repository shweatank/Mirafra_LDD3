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
    int result;
    // Attach
    int *data = (int *)shmat(shmid, NULL, 0);
    perror("shmat");

    //printf("Reader: Shared memory contains: %s\n", data);
    printf("%d\n",data[0]+data[1]);
    // Detach and optionally remove
    result=data[0]+data[1];
    *data=result;
    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);  // remove shared memory
    return 0;
}
