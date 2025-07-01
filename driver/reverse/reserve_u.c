
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define DEV_PATH "/dev/reverse"
#define WRITE_STR "heyy Hii"

int main(void)
{
    /* Open the character device */
    int fd = open(DEV_PATH, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    /* Write data */
    ssize_t written = write(fd, WRITE_STR, strlen(WRITE_STR));
    if (written < 0) {
        perror("write");
        close(fd);
        return 1;
    }
    printf("Wrote %zd bytes: \"%s\"\n", written, WRITE_STR);

    /* Reset file offset to beginning */
   // if (lseek(fd, 0, SEEK_SET) < 0) {
       // perror("lseek");
       // close(fd);
     //   return 1;
   // }


    char buf[64] = {0};
    ssize_t readn = read(fd, buf, written);
    if (readn < 0) {
        perror("read");
        close(fd);
        return 1;
    }
    buf[readn] = '\0';

    printf("Read  %zd bytes (reversed by kernel): \"%s\"\n", readn, buf);

    close(fd);
    return 0;
}

