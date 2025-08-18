#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <ctype.h>

#define OLED_I2C_ADDR 0x3C
#define OLED_WIDTH    128
#define OLED_HEIGHT   32
#define OLED_PAGES    (OLED_HEIGHT / 8)

static int i2c_fd;

// Send command to SSD1306
void ssd1306_command(uint8_t cmd) {
    uint8_t buffer[2] = {0x00, cmd};
    write(i2c_fd, buffer, 2);
}

// Send data bytes
void ssd1306_data(const uint8_t *data, size_t size) {
    uint8_t *buffer = malloc(size + 1);
    buffer[0] = 0x40;
    memcpy(buffer + 1, data, size);
    write(i2c_fd, buffer, size + 1);
    free(buffer);
}

// Init display
void ssd1306_init() {
    ssd1306_command(0xAE);
    ssd1306_command(0x20); ssd1306_command(0x00);
    ssd1306_command(0xB0);
    ssd1306_command(0xC8);
    ssd1306_command(0x00);
    ssd1306_command(0x10);
    ssd1306_command(0x40);
    ssd1306_command(0x81); ssd1306_command(0x7F);
    ssd1306_command(0xA1);
    ssd1306_command(0xA6);
    ssd1306_command(0xA8); ssd1306_command(0x1F);
    ssd1306_command(0xA4);
    ssd1306_command(0xD3); ssd1306_command(0x00);
    ssd1306_command(0xD5); ssd1306_command(0x80);
    ssd1306_command(0xD9); ssd1306_command(0xF1);
    ssd1306_command(0xDA); ssd1306_command(0x02);
    ssd1306_command(0xDB); ssd1306_command(0x40);
    ssd1306_command(0x8D); ssd1306_command(0x14);
    ssd1306_command(0xAF);
}

// Convert raw bitmap to SSD1306 page format
void convert_bitmap_to_ssd1306(const uint8_t *src, uint8_t *dst) {
    for (uint8_t page = 0; page < OLED_PAGES; page++) {
        for (uint8_t x = 0; x < OLED_WIDTH; x++) {
            uint8_t byte = 0;
            for (uint8_t bit = 0; bit < 8; bit++) {
                uint16_t y = page * 8 + bit;
                if (src[y * OLED_WIDTH + x]) {
                    byte |= (1 << bit);
                }
            }
            dst[page * OLED_WIDTH + x] = byte;
        }
    }
}

// Load 1-bit BMP
int load_bmp_mono(const char *filename, uint8_t *buffer) {
    FILE *f = fopen(filename, "rb");
    if (!f) return -1;

    uint8_t header[62];
    fread(header, 1, 62, f);

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    uint16_t bpp = *(uint16_t*)&header[28];

    if (width != OLED_WIDTH || height != OLED_HEIGHT || bpp != 1) {
        fclose(f);
        return -2;
    }

    int row_bytes = ((width + 31) / 32) * 4;
    uint8_t row[row_bytes];

    for (int y = OLED_HEIGHT - 1; y >= 0; y--) {
        fread(row, 1, row_bytes, f);
        for (int x = 0; x < width; x++) {
            int byte_index = x / 8;
            int bit_index = 7 - (x % 8);
            buffer[y * width + x] = (row[byte_index] & (1 << bit_index)) ? 1 : 0;
        }
    }

    fclose(f);
    return 0;
}

// Case-insensitive string compare
int strcasecmp_safe(const char *a, const char *b) {
    while (*a && *b) {
        char ca = tolower((unsigned char)*a);
        char cb = tolower((unsigned char)*b);
        if (ca != cb) return (ca - cb);
        a++; b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

// Sorting function for qsort
int cmp_filenames(const void *a, const void *b) {
    char * const *fa = a;
    char * const *fb = b;
    return strcasecmp_safe(*fa, *fb);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s /path/to/image_dir\n", argv[0]);
        return 1;
    }

    if ((i2c_fd = open("/dev/i2c-1", O_RDWR)) < 0) {
        perror("I2C open");
        return 1;
    }
    if (ioctl(i2c_fd, I2C_SLAVE, OLED_I2C_ADDR) < 0) {
        perror("I2C ioctl");
        return 1;
    }

    ssd1306_init();

    struct dirent *entry;
    DIR *dp = opendir(argv[1]);
    if (!dp) {
        perror("opendir");
        return 1;
    }

    char *files[256];
    int file_count = 0;

    while ((entry = readdir(dp)) != NULL) {
        size_t len = strlen(entry->d_name);
        if (len > 4 && strcasecmp_safe(entry->d_name + len - 4, ".bmp") == 0) {
            files[file_count] = strdup(entry->d_name);
            file_count++;
        }
    }
    closedir(dp);

    if (file_count == 0) {
        printf("No .bmp files found in %s\n", argv[1]);
        return 1;
    }

    qsort(files, file_count, sizeof(char*), cmp_filenames);

    uint8_t mono_bitmap[OLED_WIDTH * OLED_HEIGHT];
    uint8_t oled_buffer[OLED_WIDTH * OLED_PAGES];
    char filepath[512];

    while (1) {
        for (int i = 0; i < file_count; i++) {
            snprintf(filepath, sizeof(filepath), "%s/%s", argv[1], files[i]);

            if (load_bmp_mono(filepath, mono_bitmap) != 0) {
                printf("Failed to load %s\n", filepath);
                continue;
            }

            convert_bitmap_to_ssd1306(mono_bitmap, oled_buffer);

            for (uint8_t page = 0; page < OLED_PAGES; page++) {
                ssd1306_command(0xB0 + page);
                ssd1306_command(0x00);
                ssd1306_command(0x10);
                ssd1306_data(oled_buffer + (page * OLED_WIDTH), OLED_WIDTH);
            }

            usleep(5000); // 0.5 sec delay
        }
    }

    for (int i = 0; i < file_count; i++) {
        free(files[i]);
    }
    close(i2c_fd);
    return 0;
}
