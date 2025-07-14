#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>

#define DEVICE "/dev/mailbox_dev"

#define IOCTL_MAGIC 'M'
#define IOCTL_SET_ID _IOW(IOCTL_MAGIC, 1, int)

int main(int argc, char *argv[]) {
    int fd;
    int mailbox_id;
    ssize_t ret;
    char buf[256];

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <mailbox_id> <message>\n", argv[0]);
        return 1;
    }

    mailbox_id = atoi(argv[1]);
    const char *message = argv[2];

    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    // Set mailbox ID
    if (ioctl(fd, IOCTL_SET_ID, &mailbox_id) < 0) {
        perror("Failed to set mailbox ID");
        close(fd);
        return 1;
    }

    // Write message
    ret = write(fd, message, strlen(message));
    if (ret < 0) {
        perror("Failed to write message");
        close(fd);
        return 1;
    }

    // Read reply (blocking until message ready)
    ret = read(fd, buf, sizeof(buf) - 1);
    if (ret < 0) {
        perror("Failed to read message");
        close(fd);
        return 1;
    }
    buf[ret] = '\0';

    printf("Received from mailbox %d: %s\n", mailbox_id, buf);

    close(fd);
    return 0;
}

