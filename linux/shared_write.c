#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
int main() {
    key_t key = ftok("shmfile", 65); 
    int shmid = shmget(key, 4 * sizeof(int), 0666 | IPC_CREAT);

    int *arr = (int *)shmat(shmid, NULL, 0);

    printf("Reading array from shared memory:\n");
    for (int i = 0; i < 2; i++)
    {
        printf("arr[%d] =", i);
	scanf("%d",&arr[i]);
    }
    arr[2]=1;
  printf("Array written to shared memory.\n");
  while(arr[2]==1)
  {
	  sleep();
  }
  printf("sum from the sender:%d",arr[3]);
    shmdt(arr); 
    return 0;
}

