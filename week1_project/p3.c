// p3.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "common.h"


int msgid;
sem_t sem_add, sem_sub,sem_mul,sem_div;
// Addition thread
void* add_thread(void* arg) {
    struct msg_buffer msg;
    while (1)
    {
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long),1, 0); 
            sem_wait(&sem_add);  // Wait permission to process
            msg.result = msg.num1 + msg.num2;
	    printf("in add msg type:%ld\n",msg.msg_type);
	    
            msg.msg_type = 60;
            msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
	    printf("add message send successfully to the p1..\n");
            sem_post(&sem_add);
       
    }
}

//  Subtraction thread
void* sub_thread(void* arg) {
    struct msg_buffer msg;
    while (1) 
  {
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long),2, 0);
          
	    printf("in sub msg type:%ld\n",msg.msg_type);
            sem_wait(&sem_sub);  // Wait permission to process
            msg.result = msg.num1 - msg.num2;
	   // printf("in sub msg type:%ld\n",msg.msg_type);
            msg.msg_type = 60;

            msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
	    printf("sub message send successfully to the p1..\n");
	   	    sem_post(&sem_sub);
        

    }
}
//mul thread
void* mul_thread(void* arg)
{
    struct msg_buffer msg;
	while(1)
	{
		msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),3,0);
	   // printf("in mul msg type:%ld\n",msg.msg_type);
            sem_wait(&sem_mul);  // Wait permission to process
            msg.result = msg.num1 * msg.num2;
	    printf("in mul msg type:%ld\n",msg.msg_type);
            msg.msg_type = 60;

            msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
	    printf("mul message send successfully to the p1..\n");
	    
	    sem_post(&sem_mul);
	}
}
//div thread
void* div_thread(void* arg)
{
    struct msg_buffer msg;
	while(1)
	{
	    msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),4,0);
            sem_wait(&sem_div);  // Wait permission to process
	    printf("in div msg type:%ld\n",msg.msg_type);
            
	    msg.result = msg.num1 / msg.num2;
            msg.msg_type = 60;
            msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
	    printf("div message send successfully to the p1..\n");
	    sem_post(&sem_div);
	}
}
void *exit_thread(void *arg)
{
	struct msg_buffer msg;
	msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),99,0);
	msg.msg_type=60;
	msg.result=-1;
	msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
	if(msg.num1==-1 && msg.num2==-1)
		exit(0);
}

int main() {
    pthread_t t_add, t_sub,t_mul,t_div,t_exit;

    msgid = msgget(MSG_KEY,  0666);
    if (msgid < 0) {
        perror("msgget");
        exit(1);
    }
    sem_init(&sem_add,0,1);  // Allow immediate processing
    sem_init(&sem_sub,0,1);  // Allow immediate processing
    sem_init(&sem_mul,0,1);
    sem_init(&sem_div,0,1);

    pthread_create(&t_add, NULL, add_thread, NULL);
    pthread_create(&t_sub, NULL, sub_thread, NULL);
    pthread_create(&t_mul, NULL, mul_thread, NULL);
    pthread_create(&t_div, NULL, div_thread, NULL);
    pthread_create(&t_exit,NULL,exit_thread,NULL);


    pthread_join(t_add, NULL);
    pthread_join(t_sub, NULL);
    pthread_join(t_mul, NULL);
    pthread_join(t_div, NULL);
    pthread_join(t_exit,NULL);

    return 0;
}

