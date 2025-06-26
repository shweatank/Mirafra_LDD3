#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024

int main() {
    // Create shared memory segment
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }
    int a,b;
    printf("Enter the arguments : ");
    scanf("%d%d",&a,&b);
    // Attach to shared memory
    int *data = (int *)shmat(shmid, NULL, 0);
    perror("shmat");

    // Write data
    //strcpy(data, "Hello from writer!");
    data[0]=a;
    data[1]=b;
    printf("Writer: Wrote to shared memory.\n");
    printf("%d\n",*data);
    // Detach
    shmdt(data);
    return 0;
}
