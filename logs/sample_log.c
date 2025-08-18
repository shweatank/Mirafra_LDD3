#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//date time file-name func-name

#define FILE1 "/dev/kmsg"
#define FILE2 "/var/log/kern.log"

void myLogger(char *msg)
{
    char *logMsg = (char *)malloc(1024*sizeof(char)); 
    int fd = open(FILE1, O_WRONLY | O_APPEND, 0666);
    if (fd < 0) {
        perror("open error");
        return 1;
    }
    sprintf(logMsg, "%s %s %s %s: %s\n", __DATE__, __TIME__, __FILE__, __func__, msg);
    
    write(fd, logMsg, strlen(logMsg));
    close(fd);
}


int main()
{
    int ans = 0;
    int a = 12;
    int b = 0;

    if(b==0)
    {
        myLogger("invalid devisor");
    	return 1;
    }

    printf("ans = %d\n", a+b);
    myLogger("correct");

    return 0;
}
