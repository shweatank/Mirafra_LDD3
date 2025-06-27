//task scheduler
#include <stdio.h>
#include <unistd.h> 
typedef void (*task_fn)(void);
void task1() {
    printf("Task 1: Reading sensor data...\n");
}

void task2() {
    printf("Task 2: Processing data...\n");
}

void task3() {
    printf("Task 3: Sending data to server...\n");
}

void task4() {
    printf("Task 4: Logging system status...\n");
}

int main() {
    task_fn taskList[] = {task1, task2, task3, task4};
    int numTasks = sizeof(taskList) / sizeof(taskList[0]);
    printf("Starting simple task scheduler...\n\n");
    for (int cycle = 1; cycle <= 5; cycle++) {
        printf("=== Cycle %d ===\n", cycle);
        for (int i = 0; i < numTasks; i++) {
            taskList[i]();  
        }
        printf("\n");
        sleep(1);  
    }

    printf("Scheduler finished.\n");
    return 0;
}

