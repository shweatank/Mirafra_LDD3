#include<stdlib.h>
#include<fcntl.h>
#include<stdio.h>

#define DEVICE "/dev/enum_dev"

enum colour
{
        red=0,
        green=1,
        blue=2
};              
int main()
{       
        int fd,ret;
        fd=open(DEVICE,O_RDONLY);
        if(fd<0)
        {       
                perror("message:\n");
                exit(0);
        }       
        enum colour recv;
        ret=read(fd,&recv,sizeof(recv));
        if(ret<0)
        {       
                perror("failed\n");
                exit(0);
        }       
        printf("read successfuly from kernel:\n",recv);
}

