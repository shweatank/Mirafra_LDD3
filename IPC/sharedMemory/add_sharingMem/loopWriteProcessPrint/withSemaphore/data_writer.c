#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#define SHM_KEY 7890

#define SEMAPHORE1_NAME "/semWritten"
#define SEMAPHORE2_NAME "/semRead"

typedef struct{
    int x;
    int y;
    int result;    
} IntegerData;

int main() {

    sem_t *sem_writer = sem_open(SEMAPHORE1_NAME, O_CREAT, 0666, 0);
    if(sem_writer == SEM_FAILED)
    {
    	perror("sem_open failed");
    }    
    sem_t *sem_reader = sem_open(SEMAPHORE2_NAME, O_CREAT, 0666, 0);
    if(sem_reader == SEM_FAILED)
    {
        perror("sem_open failed");
    }
    

    // Create shared memory segment
    int shmid = shmget(SHM_KEY, sizeof(IntegerData), 0666 | IPC_CREAT);
    
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }

    // Attach to shared memory
    IntegerData *iData = (IntegerData *)shmat(shmid, NULL, 0);
    if (iData == (IntegerData *)-1) {
        perror("shmat failed");
        return 1;
    }
    
    memset(iData, 0, sizeof(IntegerData));    

    int array[10] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    int i=1;
    while(i<10)
    {
    	// Write data
            
	iData->x = array[i-1];
   	iData->y = array[i];
	i++;

	printf("Writer: Wrote to shared memory.\n");
	printf("waiting for calculator to add the data in shared memory...\n");
	
    	sem_post(sem_writer);	    
	sem_wait(sem_reader);	
	printf("result stored = %d\n", iData->result);
    }
    
    // Detach
    if (sem_close(sem_reader) == -1) 
    {
        perror("sem_close");
        return 1;
    }
    if (sem_close(sem_writer) == -1) 
    {
        perror("sem_close");
        return 1;
    }
    shmdt(iData);
    usleep(1000);
    shmctl(shmid, IPC_RMID, NULL);  // remove shared memory
    return 0;
}
