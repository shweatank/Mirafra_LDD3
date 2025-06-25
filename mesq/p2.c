#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define SHM_KEY 1234
#define SHM_SIZE 1000

int main()
{
    int shm = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    char *str = (char *)shmat(shm, NULL, 0);

    printf("write data: ");
    scanf("%[^\n]", str);

    shmdt(str);
    return 0;
}

