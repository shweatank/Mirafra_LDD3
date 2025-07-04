#ifndef SHARED_H
#define SHARED_H

// Common  Headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#define SHM_KEY 1234
typedef struct SharedData
{
    int num1;
    int num2;
    int sum;
    sem_t sem1;  // P2 (parent) → Process 3
    sem_t sem2;  // P3 → Process 2 (child)
} SharedData;

#endif

