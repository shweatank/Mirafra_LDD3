#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024

int main() {
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }

    int *x = (int *)shmat(shmid, NULL, 0);

    printf("Reader: Shared memory contains: %d %d\n",x[0],x[1]);
    x[2]=x[0]+x[1];
    x[3]=2;
    
    return 0;
}
