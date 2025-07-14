// =================== user_app.c ===================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#define DEVICE_PATH "/dev/event_logger"
#define IOCTL_GET_LOGS _IOR('e', 1, char *)
#define IOCTL_DO_LOGIN _IOW('e', 2, char *)

void read_logs() {
    int fd = open(DEVICE_PATH, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return;
    }

    char buffer[4096] = {0};
    if (ioctl(fd, IOCTL_GET_LOGS, buffer) == -1) {
        perror("IOCTL error");
        close(fd);
        return;
    }

    printf("Logs:\n%s\n", buffer);
    close(fd);
}

int do_login(const char *username, const char *password) {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Device open failed");
        return -1;
    }

    char user_input[128];
    snprintf(user_input, sizeof(user_input), "%s %s", username, password);

    int status = ioctl(fd, IOCTL_DO_LOGIN, user_input);
    close(fd);
    return status;
}

int main() {
    char username[64], password[64];

    printf("Enter Username: ");
    scanf("%s", username);
    printf("Enter Password: ");
    scanf("%s", password);

    if (do_login(username, password) != 0) {
        printf("Login failed!\n");
        return 1;
    }

    printf("Login successful. Forking process...\n");

    pid_t pid = fork();
    if (pid == 0) {
        while (1) {
            read_logs();
            sleep(5);
        }
    } else {
        wait(NULL);
    }

    return 0;
}

