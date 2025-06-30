#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

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

    while(1)
    {
    	printf("Reader: Shared memory contains: num[0] = %d, num[1] = %d\n", data[0], data[1]);


    	data[2] = data[0] + data[1];
    
    	printf("I have added numbers\n");

    	data[3] = 1;

	do
	{
	    usleep(10000);
	}while(data[3]);
    }
    // Detach and optionally remove
    shmdt(data);
    return 0;
}
