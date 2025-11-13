#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
#include <time.h>

// Constants for Raspberry Pi 4B (BCM2711)
#define GPIO_BASE_ADDR     0xFE200000  // GPIO base for RPi 4
#define BLOCK_SIZE         (4 * 1024)

#define GPIO_PIN           18          // GPIO number to blink (Pin 12)

// GPIO register offsets
#define GPFSEL_OFFSET      0x00        // Function Select registers
#define GPSET_OFFSET       0x1C        // Pin Output Set
#define GPCLR_OFFSET       0x28        // Pin Output Clear

volatile uint32_t *gpio;

void delay_ms(int ms) {
    struct timespec ts = {ms / 1000, (ms % 1000) * 1000000};
    nanosleep(&ts, NULL);
}

void setup_gpio() {
    // Set GPIO18 as output
    int reg_index = GPIO_PIN / 10;
    int bit = (GPIO_PIN % 10) * 3;

    gpio[GPFSEL_OFFSET / 4 + reg_index] &= ~(0b111 << bit); // Clear bits
    gpio[GPFSEL_OFFSET / 4 + reg_index] |=  (0b001 << bit); // Set to output (001)
}

void gpio_set(int pin) {
    gpio[GPSET_OFFSET / 4 + (pin / 32)] = (1 << (pin % 32));
}

void gpio_clear(int pin) {
    gpio[GPCLR_OFFSET / 4 + (pin / 32)] = (1 << (pin % 32));
}

int main() {
    int mem_fd;
    void *gpio_map;

    // Open /dev/mem
    mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        perror("open");
        return -1;
    }

    // Map GPIO memory
    gpio_map = mmap(
        NULL,
        BLOCK_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        mem_fd,
        GPIO_BASE_ADDR
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
    munmap((void *)gpio, BLOCK_SIZE);
    close(mem_fd);
 }

    return 0;
}
