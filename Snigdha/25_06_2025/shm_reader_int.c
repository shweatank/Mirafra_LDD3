#include<stdio.h>
#include<string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
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
    // Create shared memory segment
    int shmid = shmget(SHM_KEY, sizeof(SharedData), 0666 );
    if (shmid == -1) 
    {
        perror("shmget failed");
        return 1;
    }

    // Attach to shared memory
    SharedData *data = (SharedData *)shmat(shmid, NULL, 0);
    if (data == (void *)-1) 
    {
        perror("shmat failed");
        return 1;
    }

    // Write data
    printf("Received a:%d and b:%d from shared memory\n",data->a,data->b);
    data->result=data->a + data->b;

    printf("Wrote result to shared memory :%d\n",data->result);

    // Detach
    shmdt(data);
    return 0;
}
