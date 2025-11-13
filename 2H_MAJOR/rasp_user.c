// rasp_user.c – Cleaned Final Version with UART & LCD Feedback
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <errno.h>

#define SERIAL_DEV     "/dev/serial0"
#define BAUD_RATE      B115200
#define I2C_DEV        "/dev/i2c-1"
#define LCD_ADDR       0x27
#define FLASH_DELAY_MS 200
#define READ_TIMEOUT_S 1

static const int LED_PINS[4] = {17, 27, 22, 23};  // BCM GPIOs
static const int BUTTON_PIN  = 24;

int i2c_fd, serial_fd;
int led_fds[4], btn_fd;
volatile sig_atomic_t stop = 0;

void handle_sigint(int _) { stop = 1; }

// ------------------ GPIO Utilities ------------------
int gpio_export(int g) {
    char b[32];
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd < 0 && errno == EBUSY) return 0;
    if (fd < 0) return -1;
    int l = snprintf(b, sizeof(b), "%d", g);
    write(fd, b, l);
    close(fd);
    return 0;
}

int gpio_dir(int g, const char *d) {
    char p[64];
    snprintf(p, sizeof(p), "/sys/class/gpio/gpio%d/direction", g);
    int fd = open(p, O_WRONLY);
    if (fd < 0) return -1;
    write(fd, d, strlen(d));
    close(fd);
    return 0;
}

int gpio_open(int g, int f) {
    char p[64];
    snprintf(p, sizeof(p), "/sys/class/gpio/gpio%d/value", g);
    return open(p, f);
}

int gpio_write(int fd, int v) {
    char c = v ? '1' : '0';
    lseek(fd, 0, SEEK_SET);
    return write(fd, &c, 1) == 1 ? 0 : -1;
}

int gpio_read(int fd) {
    char c;
    lseek(fd, 0, SEEK_SET);
    return (read(fd, &c, 1) == 1) ? (c == '0' ? 0 : 1) : -1;
}

// ------------------ LCD Functions ------------------
#define RS (1<<0)
#define RW (1<<1)
#define EN (1<<2)
#define BL (1<<3)

void lcd_nib(uint8_t d) {
    uint8_t x = (d & 0xF0) | BL;
    write(i2c_fd, &x, 1); usleep(1);
    x |= EN; write(i2c_fd, &x, 1); usleep(1);
    x &= ~EN; write(i2c_fd, &x, 1); usleep(100);
}

void lcd_send(uint8_t v, int d) {
    uint8_t h = (v & 0xF0) | (d ? RS : 0) | BL;
    uint8_t l = ((v << 4) & 0xF0) | (d ? RS : 0) | BL;
    lcd_nib(h);
    lcd_nib(l);
}

void lcd_init(void) {
    if ((i2c_fd = open(I2C_DEV, O_RDWR)) < 0) { perror("i2c"); exit(1); }
    if (ioctl(i2c_fd, I2C_SLAVE, LCD_ADDR) < 0) { perror("addr"); exit(1); }
    usleep(50000);
    for (int i = 0; i < 3; i++) { lcd_nib(0x30); usleep(5000); }
    lcd_nib(0x20); usleep(1000);
    lcd_send(0x28, 0); // 4-bit 2-line
    lcd_send(0x08, 0); // Display off
    lcd_send(0x01, 0); usleep(2000);
    lcd_send(0x06, 0); // Entry mode
    lcd_send(0x0C, 0); // Display on
}

void lcd_pos(int r, int c) {
    lcd_send(0x80 | ((r ? 0x40 : 0) + c), 0);
}

void lcd_str(const char *s) {
    while (*s) lcd_send(*s++, 1);
}

// ------------------ UART ------------------
int serial_open(const char *d) {
    int fd = open(d, O_RDWR | O_NOCTTY);
    if (fd < 0) { perror("serial"); exit(1); }
    struct termios t;
    tcgetattr(fd, &t);
    cfmakeraw(&t);
    cfsetspeed(&t, BAUD_RATE);
    t.c_cflag |= CLOCAL | CREAD;
    t.c_cflag &= ~CRTSCTS;
    tcsetattr(fd, TCSANOW, &t);
    return fd;
}

// ------------------ Main ------------------
int main() {
    signal(SIGINT, handle_sigint);

    for (int i = 0; i < 4; i++) {
        gpio_export(LED_PINS[i]);
        gpio_dir(LED_PINS[i], "out");
        led_fds[i] = gpio_open(LED_PINS[i], O_WRONLY);
        gpio_write(led_fds[i], 0);
    }

    gpio_export(BUTTON_PIN);
    gpio_dir(BUTTON_PIN, "in");
    btn_fd = gpio_open(BUTTON_PIN, O_RDONLY);

    lcd_init();
    serial_fd = serial_open(SERIAL_DEV);
    printf("[rasp_user] Waiting for data on %s\n", SERIAL_DEV);

    char buf[64];
    while (!stop) {
        fd_set fds;
        struct timeval tv = {READ_TIMEOUT_S, 0};
        FD_ZERO(&fds);
        FD_SET(serial_fd, &fds);

        if (select(serial_fd + 1, &fds, NULL, NULL, &tv) > 0) {
            int n = read(serial_fd, buf, sizeof(buf) - 1);
            if (n > 0) {
                buf[n] = 0;
                if (buf[0] == '\0' || buf[0] == '\n') {
                    printf("[rasp_user] Ignored empty/null buffer\n");
                    continue;
                }

                printf("[rasp_user] UART raw: '%s'\n", buf);
                for (int i = 0; i < n; i++)
                    printf("[%02X] ", (unsigned char)buf[i]);
                printf("\n");

                int cur, max;
                if (sscanf(buf, "%d %d", &cur, &max) == 2) {
                    printf("[rasp_user] Got: cur=%d max=%d\n", cur, max);
                    lcd_pos(0, 0);
                    char t[17];
                    snprintf(t, sizeof(t), "Cur:%3d Max:%3d", cur, max);
                    lcd_str(t);
                    lcd_pos(1, 0);
                    lcd_str("Waiting     ");

                    if (cur == max) {
                        write(serial_fd, "x", 1);
                        while (gpio_read(btn_fd)) {
                            for (int j = 0; j < 4; j++) gpio_write(led_fds[j], 1);
                            usleep(FLASH_DELAY_MS * 1000);
                            for (int j = 0; j < 4; j++) gpio_write(led_fds[j], 0);
                            usleep(FLASH_DELAY_MS * 1000);
                        }
                        write(serial_fd, "z", 1);
                        for (int j = 0; j < 4; j++) gpio_write(led_fds[j], 0);
                    }
                } else {
                    printf("[rasp_user] UART format error: '%s'\n", buf);
                }
            }
        }
    }

    close(serial_fd);
    close(i2c_fd);
    for (int i = 0; i < 4; i++) close(led_fds[i]);
    close(btn_fd);
    return 0;
}

