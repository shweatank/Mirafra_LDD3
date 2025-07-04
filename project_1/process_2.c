#include "shared.h"

SharedData *data;
int shmid;
pid_t child_pid;
volatile sig_atomic_t end = 0;
volatile sig_atomic_t input_ready = 0;

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
    printf("Process 2 Child: Final Sum = %d\n", data->sum);
    // Cleanup
    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);
    exit(0);
}

int main()
{
    // Create and attach shared memory
    shmid = shmget(SHM_KEY, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        exit(1);
    }
    data = (SharedData *)shmat(shmid, NULL, 0);
    if (data == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // Initialize semaphores
    sem_init(&data->sem1, 1, 0);
    sem_init(&data->sem2, 1, 0);

    // Fork child
    pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0)
    {
        //Child Process 
        signal(SIGQUIT, child_quit);
        while (1)
            pause();
    }
    else
    {
        // Parent Process
        signal(SIGTERM, handle_term);
        signal(SIGINT, handle_sigint);
        signal(SIGQUIT, handle_sigquit);
        printf("Process 2 Parent running (PID: %d), Child PID: %d\n", getpid(), child_pid);
        while (!end) 
	{
            if (input_ready)
	    {
               input_ready = 0;

                printf("Enter two integers: ");
                scanf("%d %d", &data->num1, &data->num2);
                sem_post(&data->sem1);  // Signal Process 3
                printf("Inputs written to shared memory and signaled to Process 3\n");
            }
            pause();
        }

        // Cleanup parent
        shmdt(data);
        printf("Process 2 exiting.\n");
    }

    return 0;
}

