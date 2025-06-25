#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

int main()
{
	int fd = open("sample.txt", O_RDWR|O_CREAT, 0666);

	char data[] = "Hi this file is written using fd";

	ssize_t bytes_written = write(fd, data, sizeof(data) - 1);

	lseek(fd, 0, SEEK_SET); // rewind file offset to beginning

	char buffer[100];
        ssize_t bytes_read = read(fd, buffer, sizeof(data));

	close(fd);

	printf("read file : \n%s\n", buffer);

	return 0;
}

