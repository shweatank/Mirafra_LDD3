#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "all_images.c"
#define FB_DEVICE "/dev/ssd1306_fb"

typedef struct oled{
	int16_t width;
        int16_t height;
	uint8_t buffer[(128*32)/8];
}oled;

oled data;

int main() {
    int fd = open(FB_DEVICE, O_WRONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    data.width=128;
    data.height=32;
    ssize_t written;

    // Fill buffer with test pattern
	for (int i = 0; i < 100; i++) {
      memcpy(&data.buffer,&images[i],sizeof(images[i]));	
     written = write(fd,&data, sizeof(data));
    if (written < 0) {
        perror("write");
        close(fd);
        return 1;
    }
	}
    printf("Wrote %zd bytes to OLED\n", written);

    close(fd);
    return 0;
}

