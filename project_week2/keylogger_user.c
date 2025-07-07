#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#define DEVICE "/dev/keylogger_kernel"
#define MAGIC 'k'
#define START_LOGGING _IO(MAGIC, 1)
#define STOP_LOGGING  _IO(MAGIC, 2)
#define CLEAR_LOG     _IO(MAGIC, 3)

int main() {
    int fd;
    char buffer[1024];
    int choice;

    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Cannot open device file");
        return 1;
    }

    while (1) {
        printf("\n1. Start Logging\n2. Stop Logging\n3. View Logged Keys\n4. Clear Logs\n5. Exit\n> ");
        scanf("%d", &choice);
        getchar(); // flush newline

        switch (choice) {
            case 1:
                ioctl(fd, START_LOGGING);
                printf("Logging started.\n");
                break;
            case 2:
                ioctl(fd, STOP_LOGGING);
                printf("Logging stopped.\n");
                break;
            case 3:
                lseek(fd, 0, SEEK_SET);
                read(fd, buffer, sizeof(buffer));
                printf("Logged Keys: %s\n", buffer);
                break;
            case 4:
                ioctl(fd, CLEAR_LOG);
                printf("Logs cleared.\n");
                break;
            case 5:
                close(fd);
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}


