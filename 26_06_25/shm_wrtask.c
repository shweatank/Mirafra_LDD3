#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#define SHM_KEY 1234
#define SHM_SIZE 1024
int main()
{
   // Create shared memory segment
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }

    // Attach to shared memory
    int *data = (int*)shmat(shmid, NULL, 0);
    if (data == (int *)-1) {
        perror("shmat failed");
        return 1;
    }
    for(int i=0;i<5;i++)
    {
	    data[i]=i*5;
    }
    printf("integers written to shared memory\n");
    shmdt(data);
}

