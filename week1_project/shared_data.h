// shared_data.h
#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <pthread.h>

#define SHM_NAME "/my_shared_mem"
#define SEM_WRITE "/sem_write"
#define SEM_READ "/sem_read"
#define SEM_ITER "/sem_next_iter"

struct SharedData {
    int num1;
    int num2;
    int result;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int thread_turn;  // 0: ready to read, 1: ready to compute
};

#endif

