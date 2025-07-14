#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE_FILE "/dev/emp_dev"
#define PROC_FILE "/proc/emp_proc"
#define STATUS_FILE "/proc/emp_status"
#define IOCTL_LOGIN _IOW('a', 1, char *)

void login(int fd) {
    char password[32];
    printf("Enter password: ");
    scanf("%31s", password);

    if (ioctl(fd, IOCTL_LOGIN, password) == 0) {
        printf("Login successful.\n");
    } else {
        perror("Login failed");
        exit(EXIT_FAILURE);
    }
}

void flush_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    int dev_fd = open(DEVICE_FILE, O_RDWR);
    if (dev_fd < 0) {
        perror("Failed to open device file");
        return 1;
    }

    login(dev_fd);
    flush_stdin();  // flush leftover newline

    while (1) {
        char status[16] = {0};
        FILE *status_fp = fopen(STATUS_FILE, "r");
        if (!status_fp) {
            perror("Failed to open status file");
            break;
        }

        if (!fgets(status, sizeof(status), status_fp)) {
            perror("Failed to read from status file");
            fclose(status_fp);
            break;
        }
        fclose(status_fp);

        if (strncmp(status, "waiting", 7) == 0) {
            // Kernel is waiting for input
            char input[128];
            printf("Enter input (employee name or department): ");
            if (!fgets(input, sizeof(input), stdin)) {
                printf("Error reading input\n");
                break;
            }

           
            input[strcspn(input, "\n")] = '\0';

            FILE *proc_fp = fopen(PROC_FILE, "w");
            if (!proc_fp) {
                perror("Failed to open proc file for writing");
                break;
            }
            fprintf(proc_fp, "%s\n", input);
            fclose(proc_fp);

            sleep(1); 

            char result[512] = {0};
            FILE *res_fp = fopen(PROC_FILE, "r");
            if (!res_fp) {
                perror("Failed to open proc file for reading");
                break;
            }
            fread(result, 1, sizeof(result) - 1, res_fp);
            fclose(res_fp);

            printf("Result:\n%s\n", result);
        } else {
            // Kernel not waiting yet
            sleep(1);
        }
    }

    close(dev_fd);
    return 0;
}

