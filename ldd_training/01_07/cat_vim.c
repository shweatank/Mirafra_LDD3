#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define DEVICE "/dev/read_write_char_dev"
#define BUF_SIZE 128

int main()
{
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0)
    {
        perror("Failed to open device");
        return 1;
    }

    char write_buf[BUF_SIZE];
    char read_buf[BUF_SIZE];
    printf("Enter a string to write to the device: ");
    if (fgets(write_buf, BUF_SIZE, stdin) == NULL)
    {
        perror("Failed to read input");
        close(fd);
        return 1;
    }
     if(write_buf[strlen(write_buf)-1]=='\n')
	     write_buf[strlen(write_buf)-1]='\0';
    // Write to device
    ssize_t written = write(fd, write_buf, strlen(write_buf));
    if (written < 0)
    {
        perror("Failed to write to device");
        close(fd);
        return 1;
    }
    printf("Written %zd bytes: %s\n", written, write_buf);
    lseek(fd, 0, SEEK_SET); // Reset file offset to beginning
    ssize_t read_bytes = read(fd, read_buf, BUF_SIZE - 1);
    if (read_bytes < 0)
    {
        perror("Failed to read from device");
        close(fd);
        return 1;
    }

    read_buf[read_bytes] = '\0';
    printf("Read %zd bytes reversed string: %s\n", read_bytes, read_buf);

    close(fd);
    return 0;
}

