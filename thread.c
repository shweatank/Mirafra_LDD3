//if we want main thread to wait for other threads to finish before exiting
#include <stdio.h>
#include <pthread.h>

int var = 0;        
pthread_mutex_t lock;      

void* increment(void*) {
    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&lock);
        var++;
        printf("%d\n",var);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void* decrement(void* ) {
    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&lock);
        var--;
        printf(" %d\n",var);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_mutex_init(&lock, NULL);

    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, decrement, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&lock);

    printf("final value %d\n",var);
    return 0;
}
