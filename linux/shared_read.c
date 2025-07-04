//reader
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
int main()
{
	int sum=0;
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, 4 * sizeof(int), 0666 | IPC_CREAT);
    int *arr = (int *)shmat(shmid, (void *)0, 0);
    printf("Reading array from shared memory:\n");
    for (int i = 0; i < 2; i++)
    {
        printf("arr[%d] = %d\n", i, arr[i]);
	sum+=arr[i];
    }
    arr[3]=sum;
    arr[2]=2;
    shmdt(arr);
    return 0;
}

