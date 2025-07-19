#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CONSUMER "LED_Blink"
#define GPIO_CHIP "/dev/gpiochip0"
#define GPIO_LINE 17  // GPIO17 = pin 11 on Pi

int main() {
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    int input;

    chip = gpiod_chip_open(GPIO_CHIP);
    if (!chip) {
        perror("Open gpiochip failed");
        exit(1);
    }

    line = gpiod_chip_get_line(chip, GPIO_LINE);
    if (!line) {
        perror("Get line failed");
        gpiod_chip_close(chip);
        exit(1);
    }

    if (gpiod_line_request_output(line, CONSUMER, 0) < 0) {
        perror("Request line as output failed");
        gpiod_chip_close(chip);
        exit(1);
    }

    printf("Enter 1 to blink LED, 0 to turn it off:\n");
    while (scanf("%d", &input) == 1) {
        if (input == 1) {
            for (int i = 0; i < 5; ++i) {
                gpiod_line_set_value(line, 1);
		printf("LED SELECTED ON\n");
                sleep(1);
                gpiod_line_set_value(line, 0);
		printf("LED SET OFF\n");
		sleep(1);
	    }
        } else if (input == 0) {
            gpiod_line_set_value(line, 0);
	    printf("LED SELECTED OFF\n");
        } else {
            printf("Invalid input. Use 1 to blink or 0 to turn off.\n");
        }
        printf("Enter 1 to blink LED, 0 to turn it off:\n");
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);
    return 0;
}

