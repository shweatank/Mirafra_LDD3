#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include<unistd.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024
int *data;
int main() {
    // Create shared memory segment
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }

    // Attach to shared memory
    data = (int *)shmat(shmid, NULL, 0);
    if (data == (int *)-1) {
        perror("shmat failed");
        return 1;
    }
    /*if(fork()==0)
    {
            //printf("from process 1:\n");
            for(int i=0;i<10;i++)
            {
                    data[i]=i;
            }
    }
    else
    {
            for(int i=0;i<10;i++)
            {
                    printf("data[%d]=%d\t",i,data[i]);
            }
    }*/
    for(int i=0;i<10;i++)
    {
	    printf("data[%d]=%d\t",i,data[i]);
    }
    // Write data
    shmdt(data); 

}


