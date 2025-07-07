#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

enum Numbers
{
   first = 1,
   second,
   third,
   fourth
};

int main()
{
    int fd;
    int result = 0;
    enum Numbers num;	    

    fd = open("/dev/sampleDrv", O_RDWR);
    if(fd < 0)
    {
        perror("Failed to open device");
	return 1;
    }

    write(fd, (&num), sizeof(enum Numbers));
    
    close(fd);

    return 0;
}
