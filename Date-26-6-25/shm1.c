#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024

int main() {
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }

    int *x = (int *)shmat(shmid, NULL, 0);
    sleep(3);
    for (int i=0;i<10;i++)
    {
	    x[0]=i;
	    sleep(2);
	    printf("the messege from p2 is %d\n",x[0]);
    }


  
    shmdt(x);

    return 0;
}
