#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024

int main() {
    // Create shared memory segment
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }
    int pid;
    if((pid=fork())<0)
    {
	    perror("FORK CALL FAILED\n");
	    exit(1);
    }
    else if(pid==0)
    {
	//WRITE! CHILD
	// Attach to shared memory
	int a,b;
        int *data = (int *)shmat(shmid, NULL, 0);
        if (data == (int*)-1) 
	{
          perror("shmat failed");
          return 1;
	}

        //strcpy(data, "Hello from writer!");
        printf("Enter two numbers:\n");
	scanf("%d %d",&a,&b);

	*data = a;
	*(data+1)=b;
        printf("Writer: Wrote to shared memory.\n");
    }
    else
    {
	   //PARENT READ
	   waitpid(pid,NULL,0);
	   sleep(10);
	   int *data = (int *)shmat(shmid, NULL, 0);
           if (data == (int*)-1)
           {
            perror("shmat failed");
            return 1;
           }

	   printf("Reader ADDITION : Shared memory contains: %d\n", *data);  	
	   shmdt(data);
           shmctl(shmid, IPC_RMID, NULL);  // remove shared memory
           return 0;
	    
    }
}
