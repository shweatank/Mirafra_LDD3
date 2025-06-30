#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
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
sem_t *sem;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int input_ready = 0;
int terminate =0;

void *read_input_thread(void *arg) 
{
    while(1) {
    sem_wait(sem); 
    pthread_mutex_lock(&lock);
    if (data->exit_flag == 1) 
    {
       printf("Thread 1: Exit signal received. Exiting thread.\n");
       terminate = 1;
       pthread_cond_signal(&cond); // Wake compute_thread
       pthread_mutex_unlock(&lock);
       break;
    }
    printf("Thread 1: Received input from shared memory: %d, %d\n", data->num1, data->num2);
    input_ready=1;
    pthread_cond_signal(&cond); 
    pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void *compute_thread(void *arg) 
{
    while(1)
    {
    pthread_mutex_lock(&lock);
    while (input_ready == 0 && terminate == 0) 
    {
        pthread_cond_wait(&cond, &lock);  // Wait for input
    }
    if (terminate ==1 || data->exit_flag == 1)
    {
	    pthread_mutex_unlock(&lock);
	    printf("Thread 2: Exit signal received\n");
	    break;
    }
    int a=data->num1;
    int b=data->num2;
    char op=data->op;
    switch(op)
    {
	    case '+':data->result=a+b;break;
            case '-':data->result=a-b;break;
	    case '*':data->result=a*b;break;
	    case '/':data->result=a/b;break;
	    default:data->result=0;
	}
    data->result_ready = 1;
    input_ready=0;
    printf("Thread 2: Computed result %d %c %d= %d\n", a,op,b,data->result);
    fflush(stdout);
    pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() 
{
    pthread_t t1, t2;

    int shmid = shmget(SHM_KEY, sizeof(struct SharedData), 0666);
    if (shmid < 0) 
    {
        perror("shmget failed");
        exit(1);
    }
    data = (struct SharedData *)shmat(shmid, NULL, 0);

    sem = sem_open(SEM_NAME, 0);

    pthread_create(&t1, NULL, read_input_thread, NULL);
    pthread_create(&t2, NULL, compute_thread, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    shmdt(data);
    sem_close(sem);

    return 0;
}

