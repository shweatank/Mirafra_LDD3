#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
int main(int argc,char *argv[])
{
        int fd;
        char msg[40];
        fd=open(argv[1],O_RDONLY,0664);
        if(fd==-1)
        {
                perror("open:");
                exit(0);
        }
        struct flock var;
        //fcntl(fd,F_SETLK,var);
        var.l_type=F_RDLCK;
        var.l_whence=0;
        var.l_start=0;
        var.l_len=0;
        if((fcntl(fd,F_SETLK,&var))<0)
        {
                perror("fcnt:");
                exit(0);
        }
        printf("process 2 started READING\n");
                read(fd,msg,sizeof(msg));
                sleep(2);


        printf("process 2 completed reading\n");
        puts(msg);
        printf("=====exiting====\n");
}    
