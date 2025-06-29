// Process 3

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct data {
    int num1;
    int num2;
    int sum;
    sem_t sem;
} Data;

Data *data;
int shm_id;

// Handler to handle the signal when the SIGUSR1 is received from process 1 upon CTRL+\ is raised 
void *thread_handler(void *arg) {
    data = (Data *)arg;
    printf("Thread: Computing sum of %d and %d\n", data->num1, data->num2);
    data->sum = data->num1 + data->num2;
    printf("Thread: Computed sum = %d\n", data->sum);
    sem_post(&data->sem);  // Signal P2 child
    return NULL;
}

//Creating a thread upon reception of signal from Process 1
void signal_handler(int sig) {
    pthread_t tid;
    pthread_create(&tid, NULL, thread_handler, data); //data variable to store the data computed
    pthread_join(tid, NULL);
}

int main() {
    printf("PID of P3: %d\n", getpid());

    signal(SIGUSR1, signal_handler);
    shm_id = shmget(1, 1024, 0666 | IPC_CREAT);    //getting the shared memory ID which is created or to create shm
    if (shm_id < 0) {
        perror("shmget");
        exit(1);
    }

    data = (Data *)shmat(shm_id, NULL, 0);     //Attaching shared memory to the process
    if (data == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    while (1) pause();  // Wait for signal

    return 0;
}

