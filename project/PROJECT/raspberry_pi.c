#include <stdio.h>              
#include <fcntl.h>              // File control (for open flags)
#include <unistd.h>             // POSIX APIs (read, write, close)
#include <linux/i2c-dev.h>      // I2C definitions
#include <sys/ioctl.h>          // ioctl for configuring devices
#include <stdint.h>             // Fixed-width integer types
#include <termios.h>            // Terminal control for UART
#include <string.h>             // String handling
#include <time.h>               // For nanosleep()
#include <pthread.h>            // Threads for UART receiving
#include <stdlib.h>             // General utility
#include <gpiod.h>              // GPIO control with libgpiod

// I2C and UART device paths
#define I2C_DEVICE  "/dev/i2c-1"
#define LCD_ADDR    0x27
#define UART_DEVICE "/dev/serial0"

// LCD control bits
#define LCD_BACKLIGHT 0x08
#define EN 0x04
#define RS 0x01

// GPIO for LED
#define LED_CHIP "gpiochip0"
#define LED_LINE 17  // GPIO17 (Pin 11)

// Global variables for speed monitoring
int speed = 0;
int max_speed = 0;
int over_speed_flag = 0;
int data_received = 0;

// Mutex for thread-safe variable access
pthread_mutex_t lock;

// Millisecond sleep helper
static void msleep(int ms) {
    struct timespec ts = {ms / 1000, (ms % 1000) * 1000000};
    nanosleep(&ts, NULL);
}

// LCD pulse: triggers data latch
static void lcd_pulse(int fd, uint8_t data) {
    uint8_t temp = data | EN | LCD_BACKLIGHT;
    write(fd, &temp, 1);
    usleep(500);
    temp = (data & ~EN) | LCD_BACKLIGHT;
    write(fd, &temp, 1);
    usleep(500);
}

// Write 4 bits to LCD
static void lcd_write4(int fd, uint8_t data) {
    uint8_t temp = data | LCD_BACKLIGHT;
    write(fd, &temp, 1);
    lcd_pulse(fd, data);
}

// Send command/data byte to LCD
static void lcd_send(int fd, uint8_t byte, int is_data) {
    uint8_t high = byte & 0xF0;
    uint8_t low  = (byte << 4) & 0xF0;
    lcd_write4(fd, high | (is_data ? RS : 0));
    lcd_write4(fd, low  | (is_data ? RS : 0));
}

// Send LCD command
static void lcd_cmd(int fd, uint8_t cmd) {
    lcd_send(fd, cmd, 0);
}

// Send character to LCD
static void lcd_data(int fd, uint8_t data) {
    lcd_send(fd, data, 1);
}

// Initialize LCD in 4-bit mode
static void lcd_init(int fd) {
    msleep(50);
    lcd_write4(fd, 0x30); msleep(5);
    lcd_write4(fd, 0x30); msleep(5);
    lcd_write4(fd, 0x30); msleep(5);
    lcd_write4(fd, 0x20);
    lcd_cmd(fd, 0x28);  // 4-bit, 2-line
    lcd_cmd(fd, 0x0C);  // Display ON, cursor OFF
    lcd_cmd(fd, 0x06);  // Entry mode
    lcd_cmd(fd, 0x01);  // Clear screen
    msleep(2);
}

// Clear LCD display
static void lcd_clear(int fd) {
    lcd_cmd(fd, 0x01);
    msleep(2);
}

// Move LCD cursor
static void lcd_set_cursor(int fd, int row, int col) {
    const uint8_t row_offsets[] = {0x00, 0x40};
    lcd_cmd(fd, 0x80 | (col + row_offsets[row]));
}

// Print string to LCD
static void lcd_print(int fd, const char *s) {
    while (*s) 
    lcd_data(fd, *s++);
}

// Blink LED 3 times
void blink_led() {
    struct gpiod_chip *chip = gpiod_chip_open_by_name(LED_CHIP);
    struct gpiod_line *line = gpiod_chip_get_line(chip, LED_LINE);

    gpiod_line_request_output(line, "led", 0);

    for (int i = 0; i < 6; i++) {
        gpiod_line_set_value(line, 1);
        msleep(300);
        gpiod_line_set_value(line, 0);
        msleep(300);
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);
}

// UART reading thread
void* uart_thread(void* arg) {
    int uart = *(int*)arg;
    char buf[64];
    int pos = 0;

    while (1) {
        char ch;
        int n = read(uart, &ch, 1);
        if (n > 0) {
            if (ch == '\n' || ch == '\r') {
                buf[pos] = '\0';
                pos = 0;

                int s, m;
                if (sscanf(buf, "Speed:%d Max:%d", &s, &m) == 2) {
                    pthread_mutex_lock(&lock);
                    speed = s;
                    max_speed = m;
                    over_speed_flag = 0;
                    data_received = 1;
                    pthread_mutex_unlock(&lock);
                    printf("UART Received: Speed=%d, Max=%d\n", s, m);
                }
            } else if (pos < sizeof(buf) - 1) {
                buf[pos++] = ch;
            }
        }
        msleep(10);
    }
    return NULL;
}

int main() {
    pthread_mutex_init(&lock, NULL);

    // Open UART
    int uart = open(UART_DEVICE, O_RDWR | O_NOCTTY);
    if (uart < 0) {
        perror("UART open");
        return 1;
    }

    // Configure UART
    struct termios tty;
    tcgetattr(uart, &tty);
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8 | CREAD | CLOCAL;
    tty.c_lflag = tty.c_iflag = tty.c_oflag = 0;
    tcsetattr(uart, TCSANOW, &tty);

    // Open I2C and set LCD address
    int i2c = open(I2C_DEVICE, O_RDWR);
    if (i2c < 0) {
        perror("I2C open");
        return 1;
    }
    if (ioctl(i2c, I2C_SLAVE, LCD_ADDR) < 0) {
        perror("I2C ioctl");
        return 1;
    }

    // Initialize LCD
    lcd_init(i2c);

    // Start UART thread
    pthread_t tid;
    pthread_create(&tid, NULL, uart_thread, &uart);

    printf("Waiting for speed data from x86B...\n");

    // Wait for initial speed data
    while (!data_received)
        msleep(100);

    int local_speed = 0, local_max = 0;

    while (1) {
        pthread_mutex_lock(&lock);
        if (!over_speed_flag && speed < max_speed)
            speed++;  // Increment speed automatically
        local_speed = speed;
        local_max = max_speed;
        int alert = over_speed_flag;
        pthread_mutex_unlock(&lock);

        if (local_speed >= local_max && local_max > 0 && !alert) {
            lcd_clear(i2c);
            lcd_set_cursor(i2c, 0, 0);
            lcd_print(i2c, "Over Speed!");
            lcd_set_cursor(i2c, 1, 0);
            lcd_print(i2c, "   ALERT!   ");
            blink_led();
            system("aplay -D plughw:0,0 /home/mirafra/proj/alert.wav &");

            pthread_mutex_lock(&lock);
            over_speed_flag = 1;
            pthread_mutex_unlock(&lock);
        }

        else if (!over_speed_flag) {
            char line1[17], line2[17];
            snprintf(line1, sizeof(line1), "Speed:     %3d", local_speed);
            snprintf(line2, sizeof(line2), "MaxSpeed:  %3d", local_max);

            lcd_clear(i2c);
            lcd_set_cursor(i2c, 0, 0);
            lcd_print(i2c, line1);
            lcd_set_cursor(i2c, 1, 0);
            lcd_print(i2c, line2);
        }

        msleep(1000);  // Update every second
    }

    close(i2c);
    close(uart);
    return 0;
}

