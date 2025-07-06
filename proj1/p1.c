//Process 1

#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>

pid_t pid2,pid3;

void myhandler(int sig) // Handler to handle the signals based on the Signal number
{
        if(sig==SIGINT)
        {
                kill(pid2,SIGUSR1);
        }
        else if(sig==SIGQUIT)
        {
                kill(pid3,SIGUSR1);
        }
}

int main(int argc,char *argv[])
{
        int r,sig;
        if(argc!=3)
        {
                printf("Arguments should be: ./p1 pid_of_p2 pid_of_p3\n");
                return 0;
        }
        pid2=atoi(argv[1]);   //PID of P2      
        pid3=atoi(argv[2]);   //PID of P3
        signal(SIGINT,myhandler);  //SIGINT CTRL+C
        signal(SIGQUIT,myhandler);//SIGQUIT CTRL+\
                                  
        while(1);

}

