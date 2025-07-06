// Process 2

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

typedef struct data {
    int num1;
    int num2;
    int sum;
    sem_t sem;
} Data;

int shm_id;
Data *data;

// Handler to handle the signal when SIGUSR1 is delivered to the process 2 by process 1 upon CTRL+C
void parent_handler(int sig) {
    if (sig == SIGUSR1) {
        printf("Parent received SIGUSR1 - Writing to shared memory\n");
        data->num1 = 10;
        data->num2 = 20;
        printf("Written %d and %d into shared region\n", data->num1, data->num2);
    }
}

int main() {
    printf("PID of P2: %d\n", getpid());

    shm_id = shmget(1, 1024, 0666 | IPC_CREAT);   //creating shared memory
    if (shm_id < 0) {
        perror("shmget");
        exit(1);
    }

    data = (Data *)shmat(shm_id, NULL, 0);    //Attaching the shared memory to the process
    if (data == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    sem_init(&data->sem, 1, 0);     //Initializing the semaphore

    pid_t child_pid = fork();

    if (child_pid < 0) {
        perror("fork");
        exit(1);
    } else if (child_pid == 0) {
        // Child process
        sleep(1);  // Wait for computation
        printf("Child waiting on semaphore...\n");
        sem_wait(&data->sem);
        printf("Child: Sum read from shared memory: %d\n", data->sum);
        shmdt(data);
    } else {
        // Parent process
        signal(SIGUSR1, parent_handler);
        while (1) pause(); // Wait for signal
    }

    return 0;
}

