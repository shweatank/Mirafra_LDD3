
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024

//structure to send data to shared memory
struct data {
    int a;
    int b;
    char ch;
    int result;
    int ready;
};

struct data* var;

//writing data to shared memory
void write_data(int sig) {
    printf("Enter two numbers:\n");
    scanf("%d %d", &var->a, &var->b);
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
    printf("Enter operation (+ - * /): ");
    scanf("%c", &var->ch);
    printf("entered operator: %c\n", var->ch);
    var->ready = 1;
}

//reading data from shared memory and printing it.
void read_data(int sig) {
    printf("Result = %d\n", var->result);
    var->ready=0;
}

int main() {
    int shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
  if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    var = (struct data*)shmat(shmid, NULL, 0);
   if (var == (void*)-1) {
        perror("shmat");
        exit(1);
    }
//through child and parent process sending data
    if (fork() == 0) {
        signal(SIGQUIT, write_data);
        printf("Child PID: %d\n", getpid());
        while (1) pause();
    } else {
        signal(SIGINT, read_data);
        printf("Parent PID: %d\n", getpid());
        while (1) pause();
    }

}

