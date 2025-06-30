// p2.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "common.h"

int msgid;
int operation=1;
int op_count=0;
void handle_sigquit(int sig) {
    struct msg_buffer msg;
     if (op_count >= 4) {
        printf("[p2] Max 4 operations done. Sending exit message.\n");

        // Send exit message (type 99)
        msg.msg_type = 99;
        msg.num1 = -1;
        msg.num2 = -1;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);

        exit(0);
    }
    msg.msg_type = operation;

    printf("Enter two numbers: ");
    scanf("%d %d", &msg.num1, &msg.num2);

    msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
    printf("message send sucessfully to the process 3...\n");
    operation=(operation%4)+1;
    op_count++;
}

void handle_sigint(int sig) {
    struct msg_buffer msg;

    printf("p2  Waiting for result from p3...\n");

    msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 60, 0);
        printf("Result received from p3: %d\n", msg.result);
	  if(msg.result==-1)
	  {
		  exit(0);
	  }
}

int main() {
    pid_t pid;
    msgid = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (msgid < 0) {
        perror("msgget failed");
        exit(1);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        //  Child process — handles SIGQUIT
        struct sigaction sa_quit;
        sa_quit.sa_handler = handle_sigquit;
        sigemptyset(&sa_quit.sa_mask);
        sa_quit.sa_flags = 0;
        sigaction(SIGQUIT, &sa_quit, NULL); // Ctrl+\

        printf("[Child] PID: %d — Waiting for SIGQUIT\n", getpid());

        while (1) pause();  // Wait for signal
    } else {
        //Parent process — handles SIGINT
        struct sigaction sa_int;
        sa_int.sa_handler = handle_sigint;
        sigemptyset(&sa_int.sa_mask);
        sa_int.sa_flags = 0;
        sigaction(SIGINT, &sa_int, NULL);   // Ctrl+C

        printf("[Parent] PID: %d — Waiting for SIGINT\n", getpid());

        while (1) pause();  // Wait for signal
    }

    return 0;
}

