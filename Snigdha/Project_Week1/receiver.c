#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>

#define SHM_KEY 1234
#define SEM_NAME "/mysem"

struct SharedData 
{
    int num1;
    int num2;
    char op;
    int result_ready;
    int result;
    int exit_flag;
};

struct SharedData *data;
int shmid;
sem_t *sem;

void signal_handler(int sig) 
{
    if (sig == SIGUSR1) 
    {
        printf("Signal received by parent!\n");

	char op;

        printf("Enter two integers and operation(+,-,*,/): ");
        scanf("%d %c %d", &data->num1,&data->op, &data->num2);
	data->result_ready=0;

        sem_post(sem);
    }
}

int main() 
{
    pid_t pid;

    shmid = shmget(SHM_KEY, sizeof(struct SharedData), IPC_CREAT | 0666);
    if (shmid < 0) 
    {
        perror("shmget failed");
        exit(1);
    }
    data = (struct SharedData *)shmat(shmid, NULL, 0);
    data->result_ready = 0;
    data->exit_flag=0;

    sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);

    signal(SIGUSR1, signal_handler);

    pid = fork();
    if (pid > 0) 
    {
        printf("Parent process PID: %d\n", getpid());
	while(1)
	{
        pause();  
	if(data->exit_flag == 1)
	{
		wait(NULL);
		break;
	}
	}
    } 
    else if (pid == 0) 
    {
        printf("Child waiting for result...\n");
	fflush(stdout);

        while (1) 
	{
            if (data->result_ready == 1) 
	    {
                printf("Result from thread: %d %c %d = %d\n", data->num1,data->op, data->num2, data->result);
		fflush(stdout);
                data->result_ready = 0;
                break;
            }
            sleep(1);
        }
    }

    shmdt(data);
    if (pid > 0) 
    {
        sem_close(sem);
        sem_unlink(SEM_NAME);
        shmctl(shmid, IPC_RMID, NULL);
    }
    return 0;
}

