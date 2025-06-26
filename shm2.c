#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024


struct shm_data {
    int a;
    int b;
    int result;
};

int main() {

    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }
    struct shm_data *data = (struct shm_data*) shmat(shmid, NULL, 0);
    
    while(1){
   // sleep(1); 

    data->result = data->a + data->b;
    }

   
    //shmdt(data);
    //shmctl(shmid, IPC_RMID, NULL);  // remove shared memory
    return 0;
}
