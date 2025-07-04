#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int interrupt_flag = 0;
void signal_handler(int signum)
{
    interrupt_flag = 1;  
}

int main()
{
    signal(SIGINT, signal_handler);  
    while (!interrupt_flag) 
    {
//        printf("Waiting for interrupt...\n");
 //       sleep(1);
    }
    printf("Interrupt received!\n");
    return 0;
}

