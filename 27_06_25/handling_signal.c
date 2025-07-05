// receiver.c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/ipc.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024

struct info
{
	int a;
	int b;
	int result;
	int flag;
};
struct info *shared_data;
void handle_write(int signum)
{
	printf("entre data1:\n");
	scanf("%d",&shared_data->a);

	printf("entre data2:\n");
        scanf("%d",&shared_data->b);

	shared_data->flag=1;
}
void handle_result(int signum)
{
	printf("Final Result:%d\n",shared_data->result);
}

int main() 
{
    int ret;
     
    // Create shared memory segment
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }

    // Attach to shared memory
    shared_data = (struct info*)shmat(shmid, NULL, 0);
     if (shared_data == (void *)-1) {
        perror("shmat failed");
        return 1;
    }

    ret=fork();
    // Keep the process running to receive signals
 

    if(ret==-1)
    {
	    //printing error message
	    perror("message\n");
	    exit(0);
    }
    else if(ret>0)
    {

        signal(SIGINT, handle_write);
	signal(SIGQUIT,SIG_IGN);

	printf("Parent ID: %d\n", getpid());
	printf("Child ID:%d\n",ret);

//        wait(NULL); // wait for child

        while (1);
            shmdt(shared_data);
        
    }
    else
    {
        signal(SIGINT,SIG_IGN);

        // Ignore SIGTSTP in child
        signal(SIGQUIT, handle_result);

             // waiting for the signal
             while(1);
                     shmdt(shared_data);
    }

   
    return 0;
}

