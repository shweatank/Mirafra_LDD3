#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//date time file-name func-name

#define FILE1 "/home/mirafra/MirafraLDD3_training/logs/logfile.txt"

enum logLevels{
    LOG_ERROR = 0,
    LOG_WARN,    
    LOG_INFO,   
    LOG_DEBUG
}


#define CURRENT_LOG_LEVEL LOG_DEBUG

void myLogger(int logLevel, char *msg)
{
    char *logMsg = (char *)malloc(1024*sizeof(char)); 
    int fd = open(FILE1, O_CREATE | O_WRONLY | O_APPEND, 0666);
    if (fd < 0) {
        perror("open error");
        return 1;
    }
    sprintf(logMsg, "%s %s %s %s: %s\n", __DATE__, __TIME__, __FILE__, __func__, msg);
    
    if(logLevel == CURRENT_LOG_LEVEL)
    {
    	write(fd, logMsg, strlen(logMsg));
    }
    free(logMesg);
    close(fd);
}


int main()
{
    int ans = 0;
    int a = 12;
    int b = 0;

    if(b==0)
    {
        myLogger(logLevel, "invalid devisor");
    	return 1;
    }

    printf("ans = %d\n", a+b);
    myLogger(logLevel,"correct");

    return 0;
}
