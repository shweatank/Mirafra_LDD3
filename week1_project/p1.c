// p1.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main() {
    pid_t child_pid, parent_pid;

    printf("[p1] Enter Child PID (SIGQUIT): ");
    scanf("%d", &child_pid);

    printf("[p1] Enter Parent PID (SIGINT): ");
    scanf("%d", &parent_pid);

    while (1) {
        int choice;
        printf("\n[p1] Choose Signal:\n");
        printf("1. Send SIGQUIT (Ctrl+\\ to Child)\n");
        printf("2. Send SIGINT  (Ctrl+C to Parent)\n");
        printf("3. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                if (kill(child_pid, SIGQUIT) == -1)
                    perror("[p1] Failed to send SIGQUIT");
                else
                    printf("[p1] SIGQUIT sent to Child PID %d\n", child_pid);
                break;

            case 2:
                if (kill(parent_pid, SIGINT) == -1)
                    perror("[p1] Failed to send SIGINT");
                else
                    printf("[p1] SIGINT sent to Parent PID %d\n", parent_pid);
                break;

            case 3:
                printf("[p1] Exiting.\n");
                exit(0);

            default:
                printf("[p1] Invalid option\n");
        }

        sleep(1);  
    }

    return 0;
}

