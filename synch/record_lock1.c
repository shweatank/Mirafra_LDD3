#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<fcntl.h>
int main(int argc,char *argv[])
{
        int fd;
        char msg[20]="abgyydtw";
        fd=open(argv[1],O_WRONLY|O_CREAT|O_APPEND,0664);
        if(fd==-1)
        {
                perror("open:");
                exit(0);
        }
        struct flock var;
        //fcntl(fd,F_SETLK,var);
        var.l_type=F_WRLCK;
        var.l_whence=0;
        var.l_start=0;
        var.l_len=0;
        if((fcntl(fd,F_SETLK,&var))<0)
        {
                perror("fcntl");
                exit(0);
        }
        printf("process 1 strated writing\n");
        for(int i=0;msg[i];i++)
        {
                write(fd,msg+i,1);
                sleep(1);
        }
        printf("process 1 completed writing\n");
        printf("process 1 terminated\n");
}
    
