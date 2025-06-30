#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
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

    sem_t *sem_writer = sem_open(SEMAPHORE1_NAME, 0);
    if(sem_writer == SEM_FAILED)
    {
    	perror("sem_open failed");
    }    
    
    sem_t *sem_reader = sem_open(SEMAPHORE2_NAME, 0);
    if(sem_reader == SEM_FAILED)
    {
    	perror("sem_open failed");
    }   	
    
    // Get existing shared memory segment
    int shmid = shmget(SHM_KEY, sizeof(IntegerData), 0666);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }

    // Attach
    IntegerData *iData = (IntegerData *)shmat(shmid, NULL, 0);
    if (iData == (IntegerData *)-1) {
        perror("shmat failed");
        return 1;
    }

    int i=1;

    while(i<10)
    {
	sem_wait(sem_writer);

	    printf("Reader: Shared memory contains: x = %d, y = %d\n", iData->x, iData->y);
    	    iData->result = iData->x + iData->y;
	    printf("I have added numbers\n");
    	    i++;

	sem_post(sem_reader);
    }
    // Detach and optionally remove
    if (sem_close(sem_writer) == -1)
    {
        perror("sem_close");
        return 1;
    }
    if (sem_close(sem_reader) == -1) 
    {
        perror("sem_close");
        return 1;
    }
    shmdt(iData);
    return 0;
}
