#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define PWM_CHIP "/sys/class/pwm/pwmchip0"
#define PWM_CHANNEL "0"
#define PERIOD_NS 1000000 // 1 ms = 1kHz

void write_sysfs(const char *path, const char *value) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        perror("Open failed");
        exit(EXIT_FAILURE);
    }
    if (write(fd, value, strlen(value)) < 0) {
        perror("Write failed");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
}

int main() {
    char path[128];

    // Export PWM channel
    snprintf(path, sizeof(path), PWM_CHIP "/export");
    write_sysfs(path, PWM_CHANNEL);
    sleep(1); // Wait for sysfs to create the pwmX dir

    // Set PWM period
    snprintf(path, sizeof(path), PWM_CHIP "/pwm" PWM_CHANNEL "/period");
    char period_str[16];
    snprintf(period_str, sizeof(period_str), "%d", PERIOD_NS);
    write_sysfs(path, period_str);

    // Enable PWM output
    snprintf(path, sizeof(path), PWM_CHIP "/pwm" PWM_CHANNEL "/enable");
    write_sysfs(path, "1");

    // Vary brightness up and down
    for (int duty = 0; duty <= 100; duty += 5) {
        snprintf(path, sizeof(path), PWM_CHIP "/pwm" PWM_CHANNEL "/duty_cycle");
        char duty_str[16];
        int duty_ns = (PERIOD_NS * duty) / 100;
        snprintf(duty_str, sizeof(duty_str), "%d", duty_ns);
        write_sysfs(path, duty_str);
        printf("Brightness: %d%%\n", duty);
        usleep(100000);
    }

    for (int duty = 100; duty >= 0; duty -= 5) {
        snprintf(path, sizeof(path), PWM_CHIP "/pwm" PWM_CHANNEL "/duty_cycle");
        char duty_str[16];
        int duty_ns = (PERIOD_NS * duty) / 100;
        snprintf(duty_str, sizeof(duty_str), "%d", duty_ns);
        write_sysfs(path, duty_str);
        printf("Brightness: %d%%\n", duty);
        usleep(100000);
    }

    // Cleanup: disable and unexport
    snprintf(path, sizeof(path), PWM_CHIP "/pwm" PWM_CHANNEL "/enable");
    write_sysfs(path, "0");

    snprintf(path, sizeof(path), PWM_CHIP "/unexport");
    write_sysfs(path, PWM_CHANNEL);

    return 0;
}

