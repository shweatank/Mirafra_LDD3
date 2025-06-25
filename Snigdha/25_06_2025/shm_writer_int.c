#include <stdio.h>
#include <sys/ipc.h>

#include <sys/shm.h>
#include<unistd.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024


typedef struct
{
	int a;
	int b;
	int result;
}SharedData;


int main() 
{
    // Get existing shared memory segment
    int shmid = shmget(SHM_KEY, sizeof(SharedData), 0666 | IPC_CREAT);
    if (shmid == -1) 
    {
        perror("shmget failed");
        return 1;
    }

    // Attach
    SharedData *data = (SharedData *)shmat(shmid, NULL, 0);
    if (data == (void *)-1) 
    {
        perror("shmat failed");
        return 1;
    }


    printf("Enter two integers:");
    scanf("%d%d",&data->a,&data->b);
    printf("Writer sent a:%d and b:%d to Shared memory \n", data->a,data->b);

    while(data->result ==0)
	    usleep(1000);
    printf("Received result:%d\n",data->result);

    // Detach and optionally remove
    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);  // remove shared memory
    return 0;
}
