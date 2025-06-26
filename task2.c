#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SHM_KEY 12
//#define SHM_SIZE 100
int main() {
    // Get existing shared memory segment
    int shmid = shmget(SHM_KEY, 2*sizeof(int), 0666);
     int *data = (int *)shmat(shmid, NULL, 0);
     int sum=data[0]+data[1];
    // printf("sum=%d",sum);
    data[2]=sum;
     shmdt(data);
     shmctl(shmid,IPC_RMID,NULL);
}


