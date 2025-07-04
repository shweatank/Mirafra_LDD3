#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/shm.h>
#include <semaphore.h>
#define KEY 1234
struct SharedData
{
    int num1, num2, sum;
    sem_t sem1, sem2;
};
struct SharedData *data;
int end = 0;
void *thread1(void *arg) 
{
    printf("Thread1: recieved inputs : %d and %d\n", data->num1, data->num2);
    return NULL;
}
void *thread2(void *arg)
{
    data->sum = data->num1 + data->num2;
    printf("Thread2 calculated the sum...\n");
    return NULL;
}
void handle_sigint(int sig)
{
    sem_wait(&data->sem1);
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread1, NULL);
    pthread_join(t1, NULL);
    pthread_create(&t2, NULL, thread2, NULL);
    pthread_join(t2, NULL);
    sem_post(&data->sem2);
}
void handle_term(int sig)
{ 
	end = 1;
}
int main() 
{
    int shmid = shmget(KEY, sizeof(*data), 0666);
    data = shmat(shmid, NULL, 0);
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_term);
    printf("Process3 running with PID %d\n", getpid());
    while (!end)
	    pause();
    shmdt(data);
    return 0;
}

