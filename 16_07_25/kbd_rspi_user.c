#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include<fcntl.h>
#include<termios.h>

#define CHIP_NAME "gpiochip0"   // GPIO chip (default on Raspberry Pi)
#define GPIO_LINE 17            // BCM GPIO17
#define CONSUMER "led-blinker"  // Just a name for this app


#define DEVICE "/dev/task"

unsigned char ch;

int main() {

    int fd,ret;
    fd = open(DEVICE, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }
    struct gpiod_chip *chip;
    struct gpiod_line *line;
     int uart0 = open("/dev/serial0", O_RDWR | O_NOCTTY);
     char ch;
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

    // Open the GPIO chip
    chip = gpiod_chip_open_by_name(CHIP_NAME);
    if (!chip) {
        perror("Failed to open GPIO chip");
        return 1;
    }

    // Get the GPIO line
    line = gpiod_chip_get_line(chip, GPIO_LINE);
    if (!line) {
        perror("Failed to get GPIO line");
        gpiod_chip_close(chip);
        return 1;
    }

    // Request the line as output, initial value 0 (LED off)
    ret = gpiod_line_request_output(line, CONSUMER, 0);
    if (ret < 0) {
        perror("Failed to request GPIO line as output");
        gpiod_chip_close(chip);
        return 1;
    }

    ret=read(fd,&ch,1);
    if (ret< 0) {
        perror("Failed to read from device");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Data from device: %c\n", ch);
    switch(ch)
    {
	    case 0x1E:// Blink the LED 10 times
                     for (int i = 0; i < 10; i++) {
                    printf("LED blinking\n");
                    gpiod_line_set_value(line, 1);  // LED ON
                    sleep(1);
                    gpiod_line_set_value(line, 0);  // LED OFF
                    sleep(1);
		     }
		    break;

           default:printf("----invalid-----\n");
		   break;
    }
     // Release resources
    gpiod_line_release(line);
    gpiod_chip_close(chip);
    close(uart0);
    close(fd);
    return EXIT_SUCCESS;
}

