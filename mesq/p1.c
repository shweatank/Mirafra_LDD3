#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_KEY 1234
#define SHM_SIZE 1000

int main()
{
    int shm = shmget(SHM_KEY, SHM_SIZE, 0666);
    char *str = (char *)shmat(shm, NULL, 0);

    printf("Read data: %s", str);

    shmdt(str);
    shmctl(shm, IPC_RMID, NULL);
    return 0;
}

