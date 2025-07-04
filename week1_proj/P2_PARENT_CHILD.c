#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>
#define KEY 1234
struct SharedData
{
    int num1, num2, sum;
    sem_t sem1, sem2;
};
struct SharedData *data;
int shmid, child_pid;
int end = 0, input_ready = 0;
void handle_term(int sig)
{
       	end = 1;
}
void handle_sigint(int sig)
{
       	input_ready = 1; 
}
void handle_sigquit(int sig)
{
       	kill(child_pid, SIGQUIT);
}
void child_quit(int sig)
{
    sem_wait(&data->sem2);
    printf("Child: Sum = %d\n", data->sum);
    exit(0);
}
int main() 
{
    shmid = shmget(KEY, sizeof(*data), 0666 | IPC_CREAT);
    data = shmat(shmid, NULL, 0);
    sem_init(&data->sem1, 1, 0);
    sem_init(&data->sem2, 1, 0);
    if ((child_pid = fork()) == 0)
    {
        signal(SIGQUIT, child_quit);
        signal(SIGTERM, handle_term);
        printf("Process2 Child running with PID %d\n", getpid());
        while (!end)
	       	pause();
        exit(0);
    }
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_sigquit);
    signal(SIGTERM, handle_term);
    printf("Process2 Parent running with PID %d, Child PID %d\n", getpid(), child_pid);
    while (!end) 
    {
        pause();
        if (input_ready)
       	{
            input_ready = 0;
            printf("Enter 2 numbers: ");
            scanf("%d %d", &data->num1, &data->num2);
            sem_post(&data->sem1);
        }
    }
    kill(child_pid, SIGTERM);
    wait(NULL);
    shmdt(data);
    return 0;
}

