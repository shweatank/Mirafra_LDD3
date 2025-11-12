// ili9225_player.c - Userspace program to play RGB565 frames on ILI9225 SPI LCD

#include <stdio.h>      // Standard I/O functions (printf, fopen, fread, etc.)
#include <stdlib.h>     // Memory allocation (malloc, free) and exit()
#include <unistd.h>     // usleep(), close()
#include <fcntl.h>      // open() flags
#include <string.h>     // strerror()
#include <errno.h>      // errno for error handling
#include <time.h>       // clock_gettime()

// Device and display configuration
#define DEVICE "/dev/ili9225"         // Framebuffer device created by kernel driver
#define WIDTH 176                     // LCD width in pixels
#define HEIGHT 220                    // LCD height in pixels
#define FRAME_SIZE (WIDTH * HEIGHT * 2)  // Frame size in bytes (RGB565 = 2 bytes per pixel)
#define NUM_FRAMES 100                // Number of frames to play
#define FRAME_DELAY_US 100000         // Delay per frame in microseconds (100 ms = 10 FPS)

// Function to calculate time difference in microseconds
long timespec_diff_us(struct timespec *start, struct timespec *end) {
    return (end->tv_sec - start->tv_sec) * 1000000L +  // Seconds → microseconds
           (end->tv_nsec - start->tv_nsec) / 1000L;   // Nanoseconds → microseconds
}

int main(void)
{
    int fd;                     // File descriptor for /dev/ili9225
    char filename[256];          // Buffer to store frame filename
    unsigned char *buffer;       // Memory buffer to hold one frame
    FILE *fp;                    // File pointer for reading frame
    int i;                       // Frame counter

    // Open the framebuffer device for writing
    fd = open(DEVICE, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open " DEVICE);
        return 1;
    }

    // Allocate memory for one frame
    buffer = malloc(FRAME_SIZE);
    if (!buffer) {
        perror("malloc");
        close(fd);
        return 1;
    }

    printf("Starting playback of %d frames at ~10 FPS...\n", NUM_FRAMES);

    // Loop over each frame
    for (i = 1; i <= NUM_FRAMES; i++) {
        struct timespec ts_start, ts_end; // Timestamps to control FPS
        long elapsed_us, sleep_us;        // Time taken for frame and remaining sleep

        // Get start timestamp
        clock_gettime(CLOCK_MONOTONIC, &ts_start);

        // Build filename for the current frame
        snprintf(filename, sizeof(filename), "frames/photo-%03d.rgb", i);

        // Open the frame file
        fp = fopen(filename, "rb");
        if (!fp) {
            fprintf(stderr, "Error opening %s: %s\n", filename, strerror(errno));
            break;
        }

        // Read the entire frame into buffer
        if (fread(buffer, 1, FRAME_SIZE, fp) != FRAME_SIZE) {
            fprintf(stderr, "Error reading %s\n", filename);
            fclose(fp);
            break;
        }
        fclose(fp); // Close frame file

        // Write the frame to the LCD device
        if (write(fd, buffer, FRAME_SIZE) != FRAME_SIZE) {
            perror("write");
            break;
        }

        // Get end timestamp
        176, 220clock_gettime(CLOCK_MONOTONIC, &ts_end);

        // Calculate elapsed time and remaining sleep to maintain FPS
        elapsed_us = timespec_diff_us(&ts_start, &ts_end);
        sleep_us = FRAME_DELAY_US - elapsed_us;
        if (sleep_us > 0) usleep(sleep_us); // Sleep for remaining time if needed

        // Optional: print progress every 10 frames
        if (i % 10 == 0) {
            printf("Frame %d/%d displayed\n", i, NUM_FRAMES);
        }
    }

    // Cleanup: free memory and close device
    free(buffer);
    close(fd);
    printf("Playback finished.\n");
    return 0;
}

