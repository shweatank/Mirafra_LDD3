#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

struct Data {
    int a, b;
    int result;
    int ready;
};

int shmid;
struct Data *shared_data;

void sigusr1_handler(int sig) {
    printf("[P2] Received SIGUSR1 (from P1)\n");
}

void sigusr2_handler(int sig) {
    printf("[P2] Received SIGUSR2 (from P1)\n");
}

int main() {
    signal(SIGUSR1, sigusr1_handler);
    signal(SIGUSR2, sigusr2_handler);

    key_t key = ftok("shmfile", 65);
    shmid = shmget(key, sizeof(struct Data), 0666 | IPC_CREAT);
    shared_data = (struct Data*) shmat(shmid, NULL, 0);

    pid_t pid = fork();

    if (pid > 0) {
        // Parent Process
        while (1) {
            printf("\n[P2-Parent] Enter two integers (a b): ");
            scanf("%d %d", &shared_data->a, &shared_data->b);
            shared_data->ready = 1;

            printf("[P2-Parent] Sent data to shared memory. Waiting for result from P3...\n");

            while (shared_data->ready != 2)
                sleep(1);

            printf("[P2-Parent] Result received from P3: %d\n", shared_data->result);
            shared_data->ready = 0;
        }
    } else {
        // Child Process
        while (1) {
            if (shared_data->ready == 2) {
                printf("[P2-Child] Final Result from P3: %d\n", shared_data->result);
                shared_data->ready = 0;
            }
            sleep(1);
        }
    }

    shmdt(shared_data);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}

