// sender.c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main() {
    pid_t receiver_pid;
    pid_t receiver;

    printf("Enter Receiver PID whom to send: ");
    scanf("%d", &receiver_pid);

    kill(receiver_pid, SIGINT);
    printf("Sender: Sent SIGINT to process %d\n", receiver_pid);

    printf("Enter Receiver PID whom to send: ");
    scanf("%d", &receiver);

    kill(receiver,SIGQUIT);
    printf("Sender:Sent SIGQUIT to process %d\n",receiver);

    return 0;
}

