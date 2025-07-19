#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int main() 
{
    int uart0 = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
    if (uart0 < 0) {
        perror("UART open failed");
        return 1;
    }

    struct termios options;
    tcgetattr(uart0, &options);
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    options.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(uart0, TCSANOW, &options);

    write(uart0, "Hello UART\n", 11);
    close(uart0);
    return 0;
}
