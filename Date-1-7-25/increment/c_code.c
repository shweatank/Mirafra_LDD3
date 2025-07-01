#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd;
    int send_val = 0;
    int recv_val = 0;
    printf("Enter The To Send To Kernel :\n");
    scanf("%d",&send_val);

    fd = open("/dev/simple_char_dev", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    write(fd, &send_val, sizeof(int));

    read(fd, &recv_val, sizeof(int));

    printf("Sent to kernel: %d, Received from kernel: %d\n", send_val, recv_val);

    close(fd);
    return 0;
}

