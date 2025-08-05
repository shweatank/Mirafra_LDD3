#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 1024  // Size of shared memory in bytes
#define SHM_KEY  0x1234 // Unique key for shared memory


int main()
{
	char *shm_ptr;
	int shmid= shmget(SHM_KEY,SHM_SIZE,IPC_CREAT | 0666);
	if(shmid==-1)
	{
		perror("shmget failed\n");
		return 1;
	}

	shm_ptr=(char *) shmat(shmid,NULL,0);
	if(shm_ptr == (char *)-1)
	{
		perror("shmat failed\n");
		exit(1);
	}

	const char *msg="Hello";
	strncpy(shm_ptr,msg,SHM_SIZE);
	printf("data written : %s\n",shm_ptr);
	shmdt(shm_ptr);
	return 0;
}
