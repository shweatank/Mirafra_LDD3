#include<stdio.h>
#include<string.h>
#include<fcntl.h>
int main()
{
    char string[] = "letstypeanything";

    printf("original string ==> %s\n", string);
    
    int fd = open("/dev/sampleDrv", O_RDWR, 0666);
    
    ssize_t bytes = write(fd, string, strlen(string));

    read(fd, string, bytes);

    printf("reversed string ==> %s\n", string);

    close(fd);
    return 0;
}
