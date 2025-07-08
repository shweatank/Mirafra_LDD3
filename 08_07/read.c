#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

struct st {
	int a;
	int b;
	char ch;
	int result;
} st;

int main() {
	int fd = open("/dev/mywait", O_RDONLY);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	printf("Reading from device (will sleep until writer wakes me)...\n");

	int n = read(fd, &st, sizeof(st));
	if (n < 0) {
		perror("read");
		close(fd);
		return 1;
	}

	printf("Received from kernel: %d %c %d = %d\n", st.a, st.ch, st.b, st.result);

	close(fd);
	return 0;
}

