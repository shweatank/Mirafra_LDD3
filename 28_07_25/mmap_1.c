#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
#include <time.h>

// Constants for mmap access
#define GPIO_MEM_SIZE       (4 * 1024)

// BCM2711 GPIO base is offset 0x0 in /dev/gpiomem
#define GPIO_BASE_OFFSET    0x0

#define GPIO_PIN            18  // GPIO to blink (GPIO18 = Pin 12)

// GPIO register offsets from GPIO base (in bytes)
#define GPFSEL0             0x00
#define GPSET0              0x1C
#define GPCLR0              0x28

volatile uint32_t *gpio;

void delay_ms(int ms) {
    struct timespec ts = {ms / 1000, (ms % 1000) * 1000000};
    nanosleep(&ts, NULL);
}

void setup_gpio() {
    // Set GPIO18 as output
    int reg_index = GPIO_PIN / 10;               // Each GPFSEL controls 10 pins
    int bit = (GPIO_PIN % 10) * 3;

    gpio[reg_index] &= ~(0b111 << bit);          // Clear bits
    gpio[reg_index] |= (0b001 << bit);           // Set as output (001)
}

void gpio_set(int pin) {
    gpio[GPSET0 / 4 + (pin / 32)] = (1 << (pin % 32));
}

void gpio_clear(int pin) {
    gpio[GPCLR0 / 4 + (pin / 32)] = (1 << (pin % 32));
}

int main() {
    int mem_fd;
    void *gpio_map;

    // Open /dev/gpiomem instead of /dev/mem
    mem_fd = open("/dev/gpiomem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        perror("open(/dev/gpiomem)");
        return -1;
    }

    // Map GPIO memory (already offset within peripheral space)
    gpio_map = mmap(
        NULL,
        GPIO_MEM_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        mem_fd,
        GPIO_BASE_OFFSET
    );

    if (gpio_map == MAP_FAILED) {
        perror("mmap");
        close(mem_fd);
        return -1;
    }

    gpio = (volatile uint32_t *)gpio_map;

    setup_gpio();

    printf("Blinking LED on GPIO%d (Pin 12)...\n", GPIO_PIN);
    for (int i = 0; i < 20; i++) {
        gpio_set(GPIO_PIN);
        delay_ms(500);
        gpio_clear(GPIO_PIN);
        delay_ms(500);
    }

    // Cleanup
    munmap((void *)gpio, GPIO_MEM_SIZE);
    close(mem_fd);

    return 0;
}
