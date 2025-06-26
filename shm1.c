#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024

struct shm_data {
    int a;
    int b;
    int result;
};
int main() {
	int x=1, y=2;
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }
	
    struct shm_data *data = (struct shm_data*)shmat(shmid, NULL, 0);
    while(1){
    data->a=x;
    data->b=y;
    
    x++;
    y++;
    //sleep(2);
    printf("result %d\n", data->result);
    }

    printf("Wrote to shared memory.\n");
    //shmdt(data);
    return 0;
}
