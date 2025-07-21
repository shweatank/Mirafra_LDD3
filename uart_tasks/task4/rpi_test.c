#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <gpiod.h>

#define BUF_SIZE 128
#define CHIP_NAME "gpiochip0"
#define GPIO_LINE 17
#define CONSUMER "led-blinker"

int main() {
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    int ret;

    int uart0 = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart0 < 0) {
        perror("UART open failed");
        return 1;
    }

    struct termios options;
    tcgetattr(uart0, &options);
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_lflag = 0;
    options.c_iflag = 0;
    options.c_oflag = 0;
    tcsetattr(uart0, TCSANOW, &options);

    chip = gpiod_chip_open_by_name(CHIP_NAME);
    if (!chip) {
        perror("Failed to open GPIO chip");
        return 1;
    }

    line = gpiod_chip_get_line(chip, GPIO_LINE);
    if (!line) {
        perror("Failed to get GPIO line");
        gpiod_chip_close(chip);
        return 1;
    }

    ret = gpiod_line_request_output(line, CONSUMER, 0);
    if (ret < 0) {
        perror("Failed to request GPIO line as output");
        gpiod_chip_close(chip);
        return 1;
    }

    char buf[BUF_SIZE];
    while (1) {
        memset(buf, 0, sizeof(buf));  // Clear previous data

        ssize_t n = read(uart0, buf, sizeof(buf) - 1);

        if (n > 0) {
            buf[n] = '\0';  // Null-terminate safely
            printf("Received: %s", buf);

	    if (strstr(buf, "o")) 
	    {
		for(int i=0;i<10;i++)
		{
                    gpiod_line_set_value(line, 1);
                    printf(" -> LED ON\n");
		    sleep(1);
                    gpiod_line_set_value(line, 0);
                    printf(" -> LED OFF\n");
		    sleep(1);
		}
            } 
            else
	    {
                printf(" -> Unrecognized command\n");
            }
        }

        usleep(100000);  // 100 ms small delay
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);
    close(uart0);
    return 0;
}

