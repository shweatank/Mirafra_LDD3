#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <ctype.h>

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 32
#define SSD1306_PAGES  (SSD1306_HEIGHT / 8)
#define SSD1306_BUF_SIZE (SSD1306_WIDTH * SSD1306_PAGES)

#define FPS 10
#define DURATION 10
#define TOTAL_FRAMES (FPS * DURATION)

// Convert 1-bit bitmap to SSD1306 page format
void convert_bitmap_to_ssd1306(const uint8_t *src, uint8_t *dst) {
    for (uint8_t page = 0; page < SSD1306_PAGES; page++) {
        for (uint8_t x = 0; x < SSD1306_WIDTH; x++) {
            uint8_t byte = 0;
            for (uint8_t bit = 0; bit < 8; bit++) {
                uint16_t y = page * 8 + bit;
                if (src[y * SSD1306_WIDTH + x]) byte |= (1 << bit);
            }
            dst[page * SSD1306_WIDTH + x] = byte;
        }
    }
}

// Load 1-bit BMP into mono buffer
int load_bmp_mono(const char *filename, uint8_t *buffer) {
    FILE *f = fopen(filename, "rb");
    if (!f) return -1;

    uint8_t header[62];
    fread(header, 1, 62, f);

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    uint16_t bpp = *(uint16_t*)&header[28];

    if (width != SSD1306_WIDTH || height != SSD1306_HEIGHT || bpp != 1) {
        fclose(f);
        return -2;
    }

    int row_bytes = ((width + 31) / 32) * 4;
    uint8_t row[row_bytes];

    for (int y = SSD1306_HEIGHT - 1; y >= 0; y--) {
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

// Case-insensitive compare
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

    int fd = open("/dev/ssd1306", O_WRONLY);
    if (fd < 0) { perror("open /dev/ssd1306"); return 1; }

    // Collect .bmp files
    struct dirent *entry;
    DIR *dp = opendir(argv[1]);
    if (!dp) { perror("opendir"); return 1; }

    char *files[512];
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

    uint8_t mono_bitmap[SSD1306_WIDTH * SSD1306_HEIGHT];
    uint8_t oled_buffer[SSD1306_BUF_SIZE];
    char filepath[512];

    int total_frames = TOTAL_FRAMES;
    int idx = 0;

    for (int f = 0; f < total_frames; f++) {
        snprintf(filepath, sizeof(filepath), "%s/%s", argv[1], files[idx]);

        if (load_bmp_mono(filepath, mono_bitmap) == 0) {
            convert_bitmap_to_ssd1306(mono_bitmap, oled_buffer);
            if (write(fd, oled_buffer, SSD1306_BUF_SIZE) != SSD1306_BUF_SIZE) {
                perror("write");
                break;
            }
        }

        idx = (idx + 1) % file_count;
    //    usleep(1000000 / FPS);
    }

    for (int i = 0; i < file_count; i++) free(files[i]);
    close(fd);
    return 0;
}

