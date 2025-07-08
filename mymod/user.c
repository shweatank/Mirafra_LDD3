/* user.c – test app for the “mymod_demo” char‑device driver
 *
 * Build   : gcc -Wall -o user user.c
 * Run     : sudo ./user <a> <b> <op>
 *           op = 1 add | 2 sub | 3 mul | 4 div
 * Example : sudo ./user 12 3 4   # perform 12 / 3
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#define DEVICE_PATH "/dev/mymod_demo"

struct operation {
        int32_t a;
        int32_t b;
        int32_t op;     /* 1:+  2:-  3:*  4:/ */
        int32_t result; /* driver fills this in */
};

static void die(const char *msg)
{
        perror(msg);
        exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
        if (argc != 4) {
                fprintf(stderr, "Usage: %s <a> <b> <op[1‑4]>\n", argv[0]);
                return EXIT_FAILURE;
        }

        struct operation req;
        req.a  = strtol(argv[1], NULL, 0);
        req.b  = strtol(argv[2], NULL, 0);
        req.op = strtol(argv[3], NULL, 0);
        req.result = 0;

        int fd = open(DEVICE_PATH, O_RDWR);
        if (fd < 0) die("open");

        /* Send operands + op code to kernel */
        ssize_t n = write(fd, &req, sizeof(req));
        if (n != sizeof(req)) die("write");

        /* Read back the structure with ‘result’ filled in */
        memset(&req, 0, sizeof(req));
        n = read(fd, &req, sizeof(req));
        if (n != sizeof(req)) die("read");

        printf("Result = %d\n", req.result);

        close(fd);
        return 0;
}

