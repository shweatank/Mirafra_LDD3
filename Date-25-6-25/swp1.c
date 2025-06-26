#include <stdio.h>
#include<unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024

int main() {
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }

    int *x = (int *)shmat(shmid, NULL, 0);

    x[0]=10;
    x[1]=20;
    x[3]=1;
    printf("Writer: Wrote to shared memory.\n");

    while(x[3]!=2);
    
    printf("The result is %d\n",x[2]);
    shmdt(x);

    return 0;
}
