#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include"bitwise_ioctl.h"
#define DEVICE_PATH "/dev/bit_a"
//#define MAX_PASS_LEN 31

//#define BITWISE_IOC_MAGIC 'b'
//#define BITWISE_SET_PASSWORD _IOW(BITWISE_IOC_MAGIC, 1, char[MAX_PASS_LEN+1])
//#define BITWISE_SET_DATA _IOW(BITWISE_IOC_MAGIC, 2, uint8_t)

int main()
{
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    char password[MAX_PASS_LEN + 1];
    uint8_t bit_data = 0;

    printf("Set password (max %d chars): ", MAX_PASS_LEN);
    if (fgets(password, sizeof(password), stdin) == NULL) {
        perror("Failed to read password");
        close(fd);
        return 1;
    }
    password[strcspn(password, "\n")] = '\0';  // remove newline

    if (ioctl(fd, BITWISE_SET_PASSWORD, password) < 0) {
        perror("Failed to set password");
        close(fd);
        return 1;
    }
    printf("Password set. Now physically type the password on your keyboard to verify.\n");

    printf("Set initial bit data (hex, e.g. e6): 0x");
    if (scanf("%hhx", &bit_data) != 1) {
        fprintf(stderr, "Invalid bit data input\n");
        close(fd);
        return 1;
    }

    if (ioctl(fd, BITWISE_SET_DATA, &bit_data) < 0) {
        perror("Failed to set initial bit data");
        close(fd);
        return 1;
    }
    printf("Initial bit data set to 0x%02x\n", bit_data);

    printf("Kernel is now listening for your typed password and toggling bits with keys 'a' to 'h'.\n");
    printf("Press Ctrl+C to exit user program (kernel keeps running).\n");

    // Just wait here to keep user program alive
    while(1) {
        pause();
    }

    close(fd);
    return 0;
}
