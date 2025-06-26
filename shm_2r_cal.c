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

    printf("Reader: Shared memory contains: %d...%d\n",*data,*(data+1));

    int temp = *(data) + *(data+1);

    *data = temp;

    printf("CALCULATED VALUE SENT!!\n");

    // Detach and optionally remove
    shmdt(data);
    return 0;
}
