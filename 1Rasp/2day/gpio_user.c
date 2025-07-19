#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define GPIO_PIN "17" // GPIO17 = pin 11 on Raspberry Pi 4

void write_to_file(const char *path, const char *value) {
    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%s", value);
    fclose(fp);
}

void export_gpio() {
    write_to_file("/sys/class/gpio/export", GPIO_PIN);
    usleep(100000); // wait for sysfs to populate
}

void unexport_gpio() {
    write_to_file("/sys/class/gpio/unexport", GPIO_PIN);
}

void set_direction(const char *direction) {
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/direction", GPIO_PIN);
    write_to_file(path, direction);
}

void write_value(const char *value) {
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/value", GPIO_PIN);
    write_to_file(path, value);
}

int main() {
    int input;

    export_gpio();
    set_direction("out");

    printf("Enter 1 to blink LED, 0 to turn it off: ");
    while (scanf("%d", &input) == 1) {
        if (input == 1) {
            for (int i = 0; i < 5; ++i) {
                write_value("1");
                sleep(1);
                write_value("0");
                sleep(1);
            }
        } else if (input == 0) {
            write_value("0");
        } else {
            printf("Invalid input. Use 1 to blink or 0 to turn off.\n");
        }

        printf("Enter 1 to blink LED, 0 to turn it off: ");
    }

    write_value("0");
    unexport_gpio();
    return 0;
}

