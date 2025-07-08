#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

struct st {
	int a;
	int b;
	char ch;
	int result;
} st;

int main() {
	int fd = open("/dev/mywait", O_WRONLY);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	printf("Enter expression (e.g. 5 + 3): ");
	scanf("%d %c %d", &st.a, &st.ch, &st.b);

	write(fd, &st, sizeof(st));

	printf("Expression sent to kernel.\n");

	close(fd);
	return 0;
}

