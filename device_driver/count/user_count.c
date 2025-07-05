#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

int main() {
    int fd;
    int value = 5;

    fd = open("/dev/simple_count_dev", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    
    write(fd, &value, sizeof(value));

  
    lseek(fd, 0, SEEK_SET);

    
    read(fd, &value, sizeof(value));

    printf("Value received from kernel: %d\n", value);

    close(fd);
    return 0;
}

