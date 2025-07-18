#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define CHIP_NAME "gpiochip0"   // GPIO chip (default on Raspberry Pi)
#define GPIO_LINE 17            // BCM GPIO17
#define CONSUMER "led-blinker"  // Just a name for this app

int main() {
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    int ret;
    int uart0 = open("/dev/serial0", O_RDWR | O_NOCTTY);
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
        static char c;
    // Blink the LED 10 times
    while(1)
    {
        read(uart0, &c, sizeof(c));
        if(c=='a')
        {
        printf("LED blinking\n");
        gpiod_line_set_value(line, 1);  // LED ON
        }
        else
        {
        gpiod_line_set_value(line, 0);  // LED OFF
        }
    }

    // Release resources
    gpiod_line_release(line);
    gpiod_chip_close(chip);
        close(uart0);
    return 0;
}

                                     
