// user.c
//
// Build : gcc -Wall -o crash_user crash_user.c
// Run   : sudo ./crash_user   (will crash the kernel)

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define DEV "/dev/nod"

int main(void)
{
        int fd = open(DEV, O_WRONLY);
        if (fd < 0) {
                perror("open");
                return EXIT_FAILURE;
        }

        unsigned char cmd = 0x01;
        if (write(fd, &cmd, 1) != 1) {
                perror("write");
                close(fd);
                return EXIT_FAILURE;
        }

        /* If we ever reach here, panic failed (unlikely) */
        puts("Sent crash byte — if you can read this, panic did not happen.");
        close(fd);
        return 0;
}

