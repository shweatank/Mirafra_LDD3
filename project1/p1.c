// File: parent_child_shared_signal.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define SHM_KEY 1234
#define SEM_NAME "/sync_sem"

struct data {
    int a;
    int b;
    int result;
    int ready;
};

struct data *shm_ptr;
sem_t *sem;
pid_t child_pid;

void parent_write_handler(int sig) {
    sem_wait(sem);
    printf("Enter the numbers to perform operation:");
    scanf("%d%d",&shm_ptr->a,&shm_ptr->b);
  //  shm_ptr->a = 5;
//    shm_ptr->b = 15;
    shm_ptr->ready = 1;
    printf("Parent (PID %d) wrote a=%d, b=%d to shared memory\n", getpid(), shm_ptr->a, shm_ptr->b);
    sem_post(sem);
}

void child_read_handler(int sig) {
    sem_wait(sem);
    if (shm_ptr->ready == 2) {
        printf("Child (PID %d) read result: %d\n", getpid(), shm_ptr->result);
    } else {
        printf("Child: Result not ready yet.\n");
    }
    sem_post(sem);
}

int main() {
    int shmid;

    // Create shared memory
    shmid = shmget(SHM_KEY, sizeof(struct data), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    shm_ptr = (struct data *)shmat(shmid, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    // Create or open semaphore
    sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }
    else if (pid == 0) {
        // Child process
        signal(SIGQUIT, child_read_handler);
        printf("Child PID: %d (waiting for SIGQUIT)\n", getpid());
        while (1) pause();
    }
    else {
        // Parent process
        child_pid = pid;
        signal(SIGINT, parent_write_handler);
        printf("Parent PID: %d (waiting for SIGINT)\n", getpid());
        wait(NULL);  // Optional: waits for child
        sem_close(sem);
        sem_unlink(SEM_NAME);
        shmdt(shm_ptr);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}

