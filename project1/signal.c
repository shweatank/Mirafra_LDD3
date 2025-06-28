//sending signals to process using kill command
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main() {
    pid_t child_pid, parent_pid;
    int sig;

    printf("enter parent pid and child pid\n");
    scanf("%d %d", &parent_pid,&child_pid);
    while (1) {
        printf("\nEnter \n3 to send SIGQUIT to child\n2 to send SIGINT to parent\n0 to exit:\n");
        scanf("%d", &sig);

        if (sig == 0) {
            printf("Exiting.\n");
            break;
        } 
        else if (sig == 3) {
            kill(child_pid, SIGQUIT); 
            printf("signal sent to child\n");   
        } 
        else if (sig == 2) {
            kill(parent_pid, SIGINT);
            printf("signal sent to parent\n");
            }
         else 
            printf("Invalid input\n");
        
    }

    
}

