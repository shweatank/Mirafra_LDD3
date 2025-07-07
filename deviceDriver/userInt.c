#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

int main()
{
    int fd;
    int result = 0;
    int a = 5;	    

    fd = open("/dev/sampleDrv", O_RDWR);
    if(fd < 0)
    {
        perror("Failed to open device");
	return 1;
    }


    const char buf = '0' + a;

    write(fd, (&buf), sizeof(buf));
    lseek(fd, 0, SEEK_SET);
    
    char res;
    read(fd, &res, sizeof(res));
    
    result = res - '0';
    printf("Read from driver: %d\n", result);
    
    close(fd);

    return 0;
}
