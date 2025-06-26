#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#define SHM_KEY 12
//#define SHM_SIZE 100
int main()
{
	int shmid=shmget(SHM_KEY,2*sizeof(int),0666|IPC_CREAT);
	int *data = (int *)shmat(shmid, NULL, 0);
   printf("Enetr 2 intergers:\n");
   scanf("%d%d",&data[0],&data[1]);

    printf("Writer: Wrote to shared memory.\n");
    while(data[2]==0)
    {
	    sleep(1);
    }
    printf("sum=%d",data[2]);


    // Detach
    shmdt(data);
   shmctl(shmid,IPC_RMID,NULL);

    return 0;
}
