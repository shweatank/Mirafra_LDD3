#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <gpiod.h>
#include <string.h>

#define WIDTH   176
#define HEIGHT  220
#define FRAME_SIZE (WIDTH * HEIGHT * 2)

#define DC_PIN    24
#define RESET_PIN 25

#define FPS 10
#define FRAME_DELAY (1000000 / FPS) // microseconds

static int spi_fd;
static struct gpiod_chip *chip;
static struct gpiod_line *dc_line, *reset_line;

// ---------------- GPIO helpers ----------------
void gpio_write(struct gpiod_line *line, int value) {
    gpiod_line_set_value(line, value);
}

// ---------------- SPI write ----------------
void spi_write(uint8_t *data, int len, int is_data) {
    gpio_write(dc_line, is_data ? 1 : 0);
    write(spi_fd, data, len);
}

// Send command
void write_cmd(uint16_t cmd) {
    uint8_t buf[2] = { cmd >> 8, cmd & 0xFF };
    spi_write(buf, 2, 0);
}

// Send data
void write_data(uint16_t data) {
    uint8_t buf[2] = { data >> 8, data & 0xFF };
    spi_write(buf, 2, 1);
}

// Hardware reset
void reset_display() {
    gpio_write(reset_line, 1); usleep(10000);
    gpio_write(reset_line, 0); usleep(10000);
    gpio_write(reset_line, 1); usleep(10000);
}

// ---------------- ILI9225 initialization ----------------
void ili9225_init() {
    reset_display();

    // Power-on sequence
    write_cmd(0x10); write_data(0x0000);
    write_cmd(0x11); write_data(0x0000);
    write_cmd(0x12); write_data(0x0000);
    write_cmd(0x13); write_data(0x0000);
    write_cmd(0x14); write_data(0x0000);
    usleep(40000);

    write_cmd(0x11); write_data(0x0018);
    write_cmd(0x12); write_data(0x6121);
    write_cmd(0x13); write_data(0x006F);
    write_cmd(0x14); write_data(0x495F);
    write_cmd(0x10); write_data(0x0800);
    usleep(10000);

    write_cmd(0x11); write_data(0x103B);
    usleep(50000);

    write_cmd(0x01); write_data(0x011C);
    write_cmd(0x02); write_data(0x0100);
    write_cmd(0x03); write_data(0x1030);
    write_cmd(0x07); write_data(0x0000);

    write_cmd(0x08); write_data(0x0808);
    write_cmd(0x0F); write_data(0x0801);
    write_cmd(0x0B); write_data(0x1100);

    // Set GRAM area (full screen)
    write_cmd(0x36); write_data(0x0000);          // X start
    write_cmd(0x37); write_data(WIDTH-1);        // X end
    write_cmd(0x38); write_data(0x0000);         // Y start
    write_cmd(0x39); write_data(HEIGHT-1);       // Y end
    write_cmd(0x20); write_data(0x0000);         // GRAM X
    write_cmd(0x21); write_data(0x0000);         // GRAM Y
    write_cmd(0x22);                              // Write GRAM

    write_cmd(0x07); write_data(0x1017);         // Display ON
}

// ---------------- Set drawing window ----------------
void set_window(int x0, int y0, int x1, int y1) {
    write_cmd(0x36); write_data(x0);
    write_cmd(0x37); write_data(x1);
    write_cmd(0x38); write_data(y0);
    write_cmd(0x39); write_data(y1);
    write_cmd(0x20); write_data(x0);
    write_cmd(0x21); write_data(y0);
    write_cmd(0x22);
}

// ---------------- Display a single frame ----------------
void display_frame_file(const char *filename, int swap_bytes) {
    FILE *f = fopen(filename, "rb");
    if (!f) { perror(filename); return; }

    uint8_t *buf = malloc(FRAME_SIZE);
    if (!buf) { perror("malloc"); fclose(f); return; }

    if (fread(buf, 1, FRAME_SIZE, f) != FRAME_SIZE) {
        fprintf(stderr, "Incomplete frame: %s\n", filename);
        free(buf); fclose(f); return;
    }
    fclose(f);

    // Swap bytes if your frame is little-endian
    if (swap_bytes) {
        for (int i = 0; i < FRAME_SIZE; i += 2) {
            uint8_t tmp = buf[i];
            buf[i] = buf[i+1];
            buf[i+1] = tmp;
        }
    }

    set_window(0, 0, WIDTH-1, HEIGHT-1);
    gpio_write(dc_line, 1); // data mode

    // Send line by line
    for (int y = 0; y < HEIGHT; y++) {
        write(spi_fd, buf + y*WIDTH*2, WIDTH*2);
    }

    free(buf);
}

// ---------------- Main ----------------
int main() {
    // Init GPIO
    chip = gpiod_chip_open_by_number(0);
    dc_line = gpiod_chip_get_line(chip, DC_PIN);
    reset_line = gpiod_chip_get_line(chip, RESET_PIN);
    gpiod_line_request_output(dc_line, "ili9225", 0);
    gpiod_line_request_output(reset_line, "ili9225", 0);

    // Init SPI
    spi_fd = open("/dev/spidev0.0", O_WRONLY);
    if (spi_fd < 0) { perror("spi open"); return 1; }
    uint8_t mode = 0;
    uint32_t speed = 16000000;
    ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    ili9225_init();

    // Play frames 001-100 at 10 FPS
    int swap_bytes = 0; // set to 1 if your raw files are little-endian (rgb565le)
    for (int i = 1; i <= 100; i++) {
        char fname[64];
        sprintf(fname, "frame_%03d.raw", i);
        display_frame_file(fname, 1);
        usleep(FRAME_DELAY); // 100ms = 10 FPS
    }

    // Cleanup
    close(spi_fd);
    gpiod_chip_close(chip);

    return 0;
}

