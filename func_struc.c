#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct {
    ssize_t (*write_func)(int, const void*, size_t);
    ssize_t (*read_func)(int, void*, size_t);
} FileOps;

int main() {
    int fd;
    char wstr[100];
    char rstr[100];

    FileOps fops = {
        .write_func = write,
        .read_func = read
    };

    fd = open("ex.txt", O_WRONLY | O_CREAT, 0664);

    printf("Enter the data:\n");
    fgets(wstr, sizeof(wstr), stdin);

    fops.write_func(fd, wstr, strlen(wstr)+1);
    close(fd);

    fd = open("ex.txt", O_RDONLY);
     int  bytes = fops.read_func(fd, rstr, sizeof(rstr));

    printf("Data read from file: %s", rstr);
    close(fd);
}

