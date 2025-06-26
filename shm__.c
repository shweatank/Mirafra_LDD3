#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

struct SharedData {
    int num1;
    int num2;
    int result;
    int data_ready;
};

#define SHM_KEY 1234

int main() {
    int shmid;
    struct SharedData *shared;

    // Create shared memory
    shmid = shmget(SHM_KEY, sizeof(struct SharedData), 0666 | IPC_CREAT);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    // Attach to shared memory
    shared = (struct SharedData *) shmat(shmid, NULL, 0);
    if (shared == (void *) -1) {
        perror("shmat");
        exit(1);
    }

    // Input two integers
    printf("[Writer] Enter two integers: ");
    scanf("%d %d", &shared->num1, &shared->num2);
    shared->data_ready = 1;

    // Wait for result
    printf("[Writer] Waiting for result...\n");
    while (shared->data_ready != 2);

    printf("[Writer] Result received: %d\n", shared->result);

    // Cleanup
    shmdt(shared);
    shmctl(shmid, IPC_RMID, NULL);  // remove shared memory

    return 0;
}
