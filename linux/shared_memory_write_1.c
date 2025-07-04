//write
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
int main()
{
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, 5 * sizeof(int), 0666 | IPC_CREAT);
    int *arr = (int *)shmat(shmid, NULL, 0);
    int a = 200, b = 100;
    while (1)
    {
        if (arr[2] == 0) 
	{ 
            arr[0] = a;
            arr[1] = b;
            arr[2] = 1;
            printf("Writer sent: a = %d, b = %d\n", a, b);
            while (arr[2] == 1)
	    {
                sleep(1);
            }
            printf("Reader returned sum: %d\n", arr[3]);
            a++;
            b++;
            sleep(1);
        }
    }

    shmdt(arr);
    return 0;
}

