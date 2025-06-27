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

#define SHM_KEY 0x1234

int main() {
	int shmid;
	struct SharedData *shared;

	// Access existing shared memory
	shmid = shmget(SHM_KEY, sizeof(struct SharedData), 0666);
	if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    shared = (struct SharedData *) shmat(shmid, NULL, 0);
    if (shared == (void *) -1) {
        perror("shmat");
        exit(1);
    }

    // Wait for numbers
    while (shared->data_ready != 1)
        ;

    printf("[Adder] Read numbers: %d and %d\n", shared->num1, shared->num2);

    // Perform addition
    shared->result = shared->num1 + shared->num2;
    shared->data_ready = 2;

    printf("[Adder] Result written: %d\n", shared->result);

    // Detach
    shmdt(shared);

    return 0;
}

