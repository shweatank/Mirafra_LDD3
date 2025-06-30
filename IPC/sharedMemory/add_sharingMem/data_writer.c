#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>

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
    int *nums = (int *)shmat(shmid, NULL, 0);
    if (nums == (int *)-1) {
        perror("shmat failed");
        return 1;
    }
    
    memset(nums, 0, SHM_SIZE);    

    // Write data
    nums[0] = 5;
    nums[1] = 3;

    printf("Writer: Wrote to shared memory.\n");

    printf("waiting for calculator to add the data in shared memory...\n");

    while(!nums[3])
    {
    	usleep(10000); //to reduce CPU time for 10ms
    }

    printf("result stored = %d\n", nums[2]);
    // Detach
    shmdt(nums);
    usleep(1000);
    shmctl(shmid, IPC_RMID, NULL);  // remove shared memory
    return 0;
}
