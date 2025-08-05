#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE "/dev/mailbox_dev"
#define SYSFS_LOG "/sys/vmailbox/all_logs"
#define SYSFS_CLEAR "/sys/vmailbox/clear_logs"

#define IOCTL_MAGIC 'M'
#define IOCTL_SET_ID _IOW(IOCTL_MAGIC, 1, int)

void read_sysfs_log() {
    FILE *f = fopen(SYSFS_LOG, "r");
    if (!f) {
        perror("Failed to open sysfs log");
        return;
    }

    printf("\n--- Mailbox Logs ---\n");
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        printf("%s", line);
    }
    fclose(f);
}

void clear_sysfs_log() {
    FILE *f = fopen(SYSFS_CLEAR, "w");
    if (!f) {
        perror("Failed to clear logs");
        return;
    }
    fprintf(f, "1\n");
    fclose(f);
    printf("Logs cleared successfully.\n");
}

int main() {
    int fd;
    char buffer[256];
    int id = 2;

    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    if (ioctl(fd, IOCTL_SET_ID, &id) < 0) {
        perror("IOCTL failed");
        close(fd);
        return 1;
    }

    printf("Mailbox ID set to %d\n", id);

    write(fd, "trigger", strlen("trigger"));

    printf("Waiting for message...\n");
    ssize_t n = read(fd, buffer, sizeof(buffer));
    if (n > 0) {
        buffer[n] = '\0';
        printf("Received: %s\n", buffer);
    } else {
        perror("Read failed");
    }

    read_sysfs_log();

    // Uncomment to clear logs
    // clear_sysfs_log();

    close(fd);
    return 0;
}

