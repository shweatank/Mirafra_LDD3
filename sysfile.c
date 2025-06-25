#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

struct FileOps {
    int (*openn)(const char *, int, mode_t);
    ssize_t (*writee)(int, const void *, size_t);
    ssize_t (*readd)(int, void *, size_t);
    int (*closee)(int);
};

int main() {
    struct FileOps f = {
        .openn = open,
        .writee = write,
        .readd = read,
        .closee = close
    };

    int fd = f.openn("data.txt", O_WRONLY, 0644);
    const char *text = "Hello world";
    f.writee(fd, text, strlen(text));
    f.closee(fd);

    fd = f.openn("data.txt", O_RDONLY, 0);
    char buffer[50];
    ssize_t n = f.readd(fd, buffer, sizeof(buffer));
    buffer[n] = '\0';
    f.closee(fd);

    printf("%s\n", buffer);
    return 0;
}

