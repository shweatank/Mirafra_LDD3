//reader
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int main() {
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, 5 * sizeof(int), 0666 | IPC_CREAT);
    int *arr = (int *)shmat(shmid, NULL, 0);

    while (1) {
        if (arr[2] == 1) { // data is ready
            int a = arr[0];
            int b = arr[1];
            int sum = a + b;
            arr[3] = sum;   // return sum to writer
            arr[2] = 0;     // notify writer that processing is done

            printf("Reader received: a = %d, b = %d --> sum = %d\n", a, b, sum);
            sleep(1);
        }
    }

    shmdt(arr);
    return 0;
}

