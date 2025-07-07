#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

int add(int a, int b)
{
    return a+b;
}

int main()
{
    int (*addPtr)(int, int) = add;
    fd = open("/dev/sampleDrv", O_RDWR);
    if(fd < 0)
    {
        perror("Failed to open device");
	return 1;
    }



    write(fd, (char *)addPtr, 0);
    lseek(fd, 0, SEEK_SET);
    
    char res;
    read(fd, &res, sizeof(res));
    
    result = res - '0';
    printf("Read from driver: %d\n", result);
    
    close(fd);

    return 0;
}
