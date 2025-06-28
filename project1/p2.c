// File: thread_operations.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_KEY 1234

struct data {
    int a;
    int b;
    int result;
    int ready;
};

struct data *shm_ptr;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int a_val, b_val;

// Define function pointer type
typedef int (*arith_fn)(int, int);

// Arithmetic functions
int add(int x, int y) { return x + y; }
int sub(int x, int y) { return x - y; }
int mul(int x, int y) { return x * y; }
int divide(int x, int y) {
    if (y == 0) {
        fprintf(stderr, "Error: Division by zero\n");
        return 0;
    }
    return x / y;
}

// Thread to read values from shared memory
void* read_thread(void *arg) {
    pthread_mutex_lock(&lock);
    if (shm_ptr->ready == 1) {
        a_val = shm_ptr->a;
        b_val = shm_ptr->b;
        printf("[Thread 1] Read a=%d, b=%d from shared memory\n", a_val, b_val);
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}

// Thread to compute using function pointer
void* compute_thread(void *arg) {
    sleep(1); // ensure read completes
    arith_fn fn = (arith_fn)arg;
    int *res = malloc(sizeof(int));
    *res = fn(a_val, b_val);
    printf("[Thread 2] Computed result = %d\n", *res);
    return (void*)res;
}

// Thread to write result to shared memory
void* write_thread(void *arg) {
    sleep(1); // ensure compute completes
    int result = *(int *)arg;
    pthread_mutex_lock(&lock);
    shm_ptr->result = result;
    shm_ptr->ready = 2;
    printf("[Thread 3] Wrote result = %d to shared memory\n", result);
    pthread_mutex_unlock(&lock);
    free(arg);
    return NULL;
}

int main() {
    int shmid;
    pthread_t t1, t2, t3;
    void *res;
    int choice;

    // Connect to shared memory
    shmid = shmget(SHM_KEY, sizeof(struct data), 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    shm_ptr = (struct data *)shmat(shmid, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Display menu
    printf("Select arithmetic operation:\n");
    printf("1. Add\n2. Subtract\n3. Multiply\n4. Divide\n");
    printf("Enter choice (1-4): ");
    scanf("%d", &choice);

    // Choose operation based on user input
    arith_fn operation = NULL;
    switch (choice) {
        case 1: operation = add; break;
        case 2: operation = sub; break;
        case 3: operation = mul; break;
        case 4: operation = divide; break;
        default:
            fprintf(stderr, "Invalid choice.\n");
            exit(EXIT_FAILURE);
    }

    // Step 1: Read a, b
    pthread_create(&t1, NULL, read_thread, NULL);
    pthread_join(t1, NULL);

    // Step 2: Compute result
    pthread_create(&t2, NULL, compute_thread, (void*)operation);
    pthread_join(t2, &res);

    // Step 3: Write result back to shared memory
    pthread_create(&t3, NULL, write_thread, res);
    pthread_join(t3, NULL);

    shmdt(shm_ptr);
    return 0;
}

