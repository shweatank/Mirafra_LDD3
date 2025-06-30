#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

struct Data {
    int a, b;
    int result;
    int ready;
};

int shmid;
struct Data *shared_data;

void* receiverThread(void* arg) {
    while (1) {
        if (shared_data->ready == 1) {
            printf("[P3-Receiver] Got inputs a=%d, b=%d from shared memory\n", shared_data->a, shared_data->b);
            sleep(1); // Give time for operator thread
        }
        sleep(1);
    }
    return NULL;
}

void* operatorThread(void* arg) {
    while (1) {
        if (shared_data->ready == 1) {
            int a = shared_data->a;
            int b = shared_data->b;

            int sum = a + b;
            int diff = a - b;
            int mul = a * b;

            printf("[P3-Operator] a+b=%d, a-b=%d, a*b=%d\n", sum, diff, mul);
            shared_data->result = sum; // Sending only sum for simplicity
            shared_data->ready = 2;    // Set flag for P2 to read
        }
        sleep(1);
    }
    return NULL;
}

int main() {
    key_t key = ftok("shmfile", 65);
    shmid = shmget(key, sizeof(struct Data), 0666);
    shared_data = (struct Data*) shmat(shmid, NULL, 0);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, receiverThread, NULL);
    pthread_create(&t2, NULL, operatorThread, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    shmdt(shared_data);
    return 0;
}

