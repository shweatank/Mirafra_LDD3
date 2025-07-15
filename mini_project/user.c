/* user_temp_app.c - Updated User-space app for virtual temperature sensor */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>

#define DEVICE_FILE "/dev/virtual_temp"

#define TEMP_IOC_MAGIC  'T'
#define IOCTL_SET_THRESHOLD  _IOW(TEMP_IOC_MAGIC, 1, int)
#define IOCTL_TRIGGER_TEMP   _IO(TEMP_IOC_MAGIC, 2)
#define IOCTL_GET_CURRENT    _IOR(TEMP_IOC_MAGIC, 3, int)
#define IOCTL_STABILIZE      _IO(TEMP_IOC_MAGIC, 4)

int dev_fd;
int threshold;

void *block_until_threshold(void *arg) {
    printf("[INFO] Waiting for temperature to reach threshold...\n");
    read(dev_fd, NULL, 0);
    printf("[EVENT] Threshold crossed!\n");
    return NULL;
}

void stabilize_temperature() {
    printf("\n[INFO] Press ENTER to stabilize temperature\n");
    getchar();
    ioctl(dev_fd, IOCTL_STABILIZE);
    printf("[ACTION] Temperature set to threshold - 10 (%d)\n", threshold - 10);
}

int main() {
    pthread_t thread_id;
    int temp;

    dev_fd = open(DEVICE_FILE, O_RDWR);
    if (dev_fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    printf("Enter threshold temperature to set: ");
    scanf("%d", &threshold);
    getchar(); // consume newline after scanf
    ioctl(dev_fd, IOCTL_SET_THRESHOLD, &threshold);
    printf("[INFO] Threshold set to %d\n", threshold);

    pthread_create(&thread_id, NULL, block_until_threshold, NULL);

    while (1) {
        printf("\nOptions:\n1. Trigger Temperature\n2. Get Current Temperature\n3. Stabilize\n4. Exit\nChoice: ");
        int choice;
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                ioctl(dev_fd, IOCTL_TRIGGER_TEMP);
                printf("[ACTION] Temperature increased manually.\n");
                break;
            case 2:
                ioctl(dev_fd, IOCTL_GET_CURRENT, &temp);
                printf("[INFO] Current temperature: %d\n", temp);
                break;
            case 3:
                stabilize_temperature();
                break;
            case 4:
                close(dev_fd);
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    close(dev_fd);
    return 0;
}
