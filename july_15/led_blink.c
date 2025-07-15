#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

#define CHIP_NAME "gpiochip0"   // GPIO chip (default on Raspberry Pi)
#define GPIO_LINE 17            // BCM GPIO17
#define CONSUMER "led-blinker"  // Just a name for this app

int main() {
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    int ret;

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

    // Blink the LED 10 times
    for (int i = 0; i < 10; i++) {
	printf("LED blinking\n");
        gpiod_line_set_value(line, 1);  // LED ON
        sleep(1);
        gpiod_line_set_value(line, 0);  // LED OFF
        sleep(1);
    }

    // Release resources
    gpiod_line_release(line);
    gpiod_chip_close(chip);

    return 0;
}

