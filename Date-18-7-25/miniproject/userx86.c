// File: x86_uart_sender.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define UART_DEVICE "/dev/ttyUSB0" // Change as needed

int main() 
{
    int fd;
    struct termios options;

    fd = open(UART_DEVICE, O_WRONLY | O_NOCTTY);
    if (fd < 0) {
        perror("UART open failed");
        return 1;
    }

    tcgetattr(fd, &options);
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CRTSCTS;
    tcsetattr(fd, TCSANOW, &options);
     
    char c[128];
    while(1)
    {
	memset(c,0,128);
	printf("Enter The Command:\n");
	scanf(" %s",c);
    	write(fd, c, strlen(c));
    	printf("Sent: %s",c);
    }
    close(fd);
    return 0;
}

