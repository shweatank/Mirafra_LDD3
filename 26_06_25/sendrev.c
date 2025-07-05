#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#define SHM_KEY 1234
#define SHM_SIZE 1024
int main()
{
	int ret,total;
	ret=fork();
	if(ret==-1)
	{
		perror("message");
		exit(0);
	}
   // Create shared memory segment
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }

    // Attach to shared memory
    int *data = (int*)shmat(shmid, NULL, 0);
    if (data == (int *)-1) {
        perror("shmat failed");
        return 1;
    }
    else if(ret==0)
    {
	    printf("reading from shared memory\n");
	    for(int i=0;i<5;i++)
	    {
		    total=total+data[i];
	    }
	    printf("------updating--------\n");
	    *data=total;
    }
    else
    {
	    for(int i=0;i<5;i++)
	    {
		    data[i]=i*3;
	    }
	    printf("written to shared memory\n");
	    printf("waiting for child\n");
	    wait(NULL);
	    printf("reader:%d is the total\n",*data);
    }
}


    

