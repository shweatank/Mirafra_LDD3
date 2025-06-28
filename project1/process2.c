
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024
int calculator(int ,int,char);

pthread_mutex_t lock; //for synchronization among threads

//structure for data to receive from shared memory
struct data {
    int a;
    int b;
    char ch;
    int result;
    int ready;
};

struct data *var;
int shmid;
int result;

//function to receive data from shared memory by thread
void* data_receive(void* arg) {
    pthread_mutex_lock(&lock);
    shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT|0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    var = (struct data*)shmat(shmid, NULL, 0);
    if (var == (void*)-1) {
        perror("shmat");
        exit(1);
    }
   while (var->ready != 1) {
    pthread_mutex_unlock(&lock);
    pthread_mutex_lock(&lock);
   }
    printf("Received: a=%d, b=%d, ch=%c, ready=%d\n", var->a, var->b, var->ch, var->ready);
    result = calculator(var->a, var->b, var->ch);
    
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

//sending back the result to process1
void* data_send(void* arg) {
    pthread_mutex_lock(&lock);
    var->result = result;
	printf("result is %d\n", var->result);
	var->ready = 0;
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

int main() {
    pthread_t t1, t2;
    pthread_mutex_init(&lock, NULL);

    while (1) {
        pthread_create(&t1, NULL, data_receive, NULL);//thread to read data
        pthread_join(t1, NULL);

        pthread_create(&t2, NULL, data_send, NULL);//to send back result to process1
        pthread_join(t2, NULL);

        sleep(1);
    }

    pthread_mutex_destroy(&lock);
    return 0;
}
int calculator(int a,int b,char ch){
	switch(ch){
		case '+':return a+b;
		case '-':return a-b;
		case '*':return a*b;
		case '/':if(b!=0)
			    return a/b;
			 else{
			 printf("b should !=0\n");
			 return 0;
			 }
		default:printf("invalid operator\n");
			return 0;
	}			 		
}

