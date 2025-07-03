#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define DEVICE "/dev/enum_char_dev"
#define BUF_SIZE 128
enum operation_mode 
{
    MODE_NONE = 0,
    MODE_REVERSE,
    MODE_UPPERCASE
};

int main() 
{
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    int mode;
    printf("Select Mode:1 - Reverse\n2- Uppercase\nEnter mode: ");
    scanf("%d", &mode);
    getchar();
    // Send the enum valueas an int to the driver
    if (write(fd, &mode, sizeof(int)) < 0) 
    {
        perror("Failed to write mode to device");
        close(fd);
        return 1;
    }

    // Now send string to be processed
    char input[BUF_SIZE];
    printf("Enter a string: ");
    if (fgets(input, BUF_SIZE, stdin) == NULL)
    {
        perror("Failed to read string");
        close(fd);
        return 1;
    }
    if (input[strlen(input) - 1] == '\n')
        input[strlen(input) - 1] = '\0';

    if (write(fd, input, strlen(input)) < 0)
    {
        perror("Failed to write string to device");
        close(fd);
        return 1;
    }

    // Read 
    lseek(fd, 0, SEEK_SET);
    char output[BUF_SIZE];
    ssize_t bytes_read = read(fd, output, BUF_SIZE - 1);
    if (bytes_read < 0) {
        perror("Failed to read from device");
        close(fd);
        return 1;
    }

    output[bytes_read] = '\0';
    printf("Processed string from driver: %s\n", output);

    close(fd);
    return 0;
}

