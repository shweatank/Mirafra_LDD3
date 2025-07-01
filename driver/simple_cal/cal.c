#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define DEV_PATH "/dev/simple_calc_dev"

static int do_op(const char *op, long a, long b)
{
	char cmd[64], buf[64];
	ssize_t n;
	int fd = open(DEV_PATH, O_RDWR);
	if (fd < 0) { perror("open"); return -1; }

	snprintf(cmd, sizeof(cmd), "%s %ld %ld", op, a, b);

	if (write(fd, cmd, strlen(cmd)) < 0) {
		perror("write");
		close(fd); return -1;
	}
	/* rewind not required – driver does it */

	n = read(fd, buf, sizeof(buf) - 1);
	if (n < 0) { perror("read"); close(fd); return -1; }

	buf[n] = '\0';
	printf("%s(%ld, %ld) = %s", op, a, b, buf);

	close(fd);
	return 0;
}

int main(void)
{
	do_op("add", 10, 3);
	do_op("sub", 10, 3);
	do_op("mul", 10, 3);
	do_op("div", 10, 3);
	do_op("div", 10, 0);   /* should fail (division by zero) */
	return 0;
}

