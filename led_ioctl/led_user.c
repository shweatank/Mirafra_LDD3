#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define LED_MAGIC 'L'
#define LED_ON    _IO(LED_MAGIC, 0)
#define LED_OFF   _IO(LED_MAGIC, 1)

int main() {
    int fd;
    char choice;

    fd = open("/dev/led_gpio", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    while (1) {
        printf("Enter 1 to turn LED ON, 0 to turn it OFF, q to quit: ");
        scanf(" %c", &choice);

        if (choice == '1') {
            ioctl(fd, LED_ON);
        } else if (choice == '0') {
            ioctl(fd, LED_OFF);
        } else if (choice == 'q' || choice == 'Q') {
            printf("Exiting LED control loop.\n");
            break;
        } else {
            printf("Invalid input. Please enter 1, 0, or q.\n");
        }
    }

    close(fd);
    return 0;
}

