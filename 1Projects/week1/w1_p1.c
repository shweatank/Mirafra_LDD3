// p1.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024

volatile sig_atomic_t flag = 0;
int check_count = 0;

void sig_handler(int sig) {
    flag = 1;
}

int main() {
    struct sigaction sa;
    sa.sa_handler = sig_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    int *data = (int *)shmat(shmid, NULL, 0);
    if (data == (int *)-1) {
        perror("shmat");
        exit(1);
    }

    printf("Process 1 running (PID %d). Press Ctrl+C to input numbers.\n", getpid());

    while (1) {
        if (flag) {
            flag = 0;

            if (check_count >= 4) {
                printf("All operations done. Exiting P1.\n");
                shmdt(data);
                exit(0);
            }

            // Wait until previous operation is processed
            while (data[0] != -1) {
                usleep(100000); // wait for P2 to reset flag
            }

            int a, b;
            char input[100];
            printf("\nEnter two integers: ");
            fflush(stdout);

            if (!fgets(input, sizeof(input), stdin)) continue;
            if (sscanf(input, "%d %d", &a, &b) != 2) {
                printf("Invalid input. Try again.\n");
                continue;
            }

            data[1] = a;
            data[2] = b;
            data[0] = check_count; // operation code (0: add, ..., 3: div)

            const char *ops[] = {"Addition", "Subtraction", "Multiplication", "Division"};
            printf("%s input sent: %d, %d\n", ops[check_count], a, b);

            check_count++;
        }

        usleep(100000);
    }

    return 0;
}

