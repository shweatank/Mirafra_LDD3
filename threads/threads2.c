#include <stdio.h>
#include<unistd.h>
#include <pthread.h>

int x=1;

void* Thread_One(void* arg) {
    printf("Hello from thread 1..!\n");
    for(int i=0;i<5;i++)
    {
	    printf("%d\n",x++);
	    sleep(1);
    }
    return NULL;
}

void* Thread_One(void* arg) {
    printf("Hello from thread 1..!\n");
    for(int i=0;i<5;i++)
    {
            printf("%d\n",x++);
            sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t thread_id;
    printf("In main\n");
    pthread_create(&thread_id, NULL, Thread_One, NULL);
    pthread_join(thread_id, NULL);
    printf("Returned to Main thread\n");
    return 0;
}

