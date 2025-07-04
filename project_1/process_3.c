#include "shared.h"

SharedData *data;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
volatile sig_atomic_t end = 0;

void *reader_thread(void *arg) 
{
    pthread_mutex_lock(&lock);//locking the shm with mutex
    printf("Process 3 Thread 1: Received inputs: %d and %d\n", data->num1, data->num2);
    pthread_mutex_unlock(&lock);//unlocking 
    return NULL;
}

void *writer_thread(void *arg) 
{
    pthread_mutex_lock(&lock);//locking
    data->sum = data->num1 + data->num2;
    printf("Process 3 Thread 2: Computed sum = %d\n", data->sum);
    printf("now notify process 2 child\n");
    pthread_mutex_unlock(&lock);//unlocking
    return NULL;
}

void handle_sigint(int sig) 
{
    sem_wait(&data->sem1);  // Wait for Process 2's input

    pthread_t t1, t2;
    pthread_create(&t1, NULL, reader_thread, NULL);
    pthread_join(t1, NULL);

    pthread_create(&t2, NULL, writer_thread, NULL);
    pthread_join(t2, NULL);

    sem_post(&data->sem2);  // Notify Process 2 child
}

void handle_term(int sig)
{
    end = 1;
}

int main()
{
    int shmid = shmget(SHM_KEY, sizeof(SharedData), 0666);
    if (shmid < 0) 
    {
        perror("shmget failed");
        exit(1);
    }

    data = (SharedData *)shmat(shmid, NULL, 0);
    if (data == (void *)-1)
    {
        perror("shmat failed");
        exit(1);
    }

    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_term);

    printf("Process 3 running (PID: %d)\n", getpid());
    while (!end)
        pause();

    shmdt(data);
    return 0;
}

