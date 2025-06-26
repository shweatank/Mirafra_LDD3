#include<stdio.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#define KEY 12345
#define SHM_SIZE 1024
int main()
{
	int shmid=shmget(KEY,SHM_SIZE,0);
	if(shmid==-1)
	{
		perror("shmget");
		return -1;
	}
	int *shm_ptr=(int *)shmat(shmid,NULL,0);
	printf("%d\n",shm_ptr[0]);
	printf("%d\n",shm_ptr[1]);
	shm_ptr[2]=shm_ptr[0]+shm_ptr[1];
	return 0;
}
