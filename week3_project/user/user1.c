#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define IOCTL_CLEAR _IO('k', 1)
#define IOCTL_MODE  _IOW('k', 2, int)

#define DEVICE "/dev/virtkeydev"
#define MAX_BUF 2048

volatile int stop_thread = 0;
pthread_t reader_tid;
int reader_running = 0;

void print_user_timestamp() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    char time_buf[64];
    struct tm tm_val;
    localtime_r(&ts.tv_sec, &tm_val);
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &tm_val);
    printf("\n[USER-TIME %s.%09ld]\n", time_buf, ts.tv_nsec);
}

void *read_thread(void *arg) {
    int fd = open(DEVICE, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device for reading");
        return NULL;
    }

    char buf[MAX_BUF];
    static long long last_ts_sec = 0;
    static long last_ts_nsec = 0;

    while (!stop_thread) {
        ssize_t len = read(fd, buf, MAX_BUF - 1);
        if (len > 0) {
            if (len >= MAX_BUF)
                len = MAX_BUF - 1;
            buf[len] = '\0';

            print_user_timestamp();

            char *line = strtok(buf, "\n");
            while (line) {
                char key;
                long long sec;
                long nsec;

                if (sscanf(line, "Key: %c | Time: %lld.%ld", &key, &sec, &nsec) == 3) {
                    if (sec > last_ts_sec || (sec == last_ts_sec && nsec > last_ts_nsec)) {
                        printf("Key: %c \t", key);

                        time_t t = (time_t)sec;
                        struct tm tm_val;
                        char time_buf[64];

                        localtime_r(&t, &tm_val);
                        strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &tm_val);

                        printf(" | [KERNEL-STR-TIME %s.%09ld]\n", time_buf, nsec);

                        last_ts_sec = sec;
                        last_ts_nsec = nsec;
                    }
                }

                line = strtok(NULL, "\n");
            }
        }

        sleep(1);
    }

    close(fd);
    return NULL;
}

int main() {
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    int choice;
    while (1) {
        printf("\nEnter Command:\n1) Clear buffer\n2) Start reading\n3) Stop reading and exit\n> ");
        if (scanf("%d", &choice) != 1) break;

        if (choice == 1) {
            printf("Sending IOCTL_CLEAR...\n");
            if (ioctl(fd, IOCTL_CLEAR) < 0)
                perror("IOCTL_CLEAR failed");
        } else if (choice == 2) {
            if (reader_running) {
                printf("Read thread already running.\n");
            } else {
                stop_thread = 0;
                if (pthread_create(&reader_tid, NULL, read_thread, NULL) == 0) {
                    reader_running = 1;
                } else {
                    perror("Failed to create thread");
                }
            }
        } else if (choice == 3) {
            stop_thread = 1;
            if (reader_running) {
                pthread_join(reader_tid, NULL);
                reader_running = 0;
            }
            printf("Exiting.\n");
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }

    close(fd);
    return 0;
}

