#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

int main()
{
	int fd;
	char read_buf[100];
	const char *write_str ="Hello from user space\n";

	fd = open("/dev/sample_driver", O_RDWR);
	if(fd < 0)
	{
		perror("Failed to opern\n");
		return 1;
	}

	write(fd,write_str,strlen(write_str));
	lseek(fd,0, SEEK_SET);

	read(fd,read_buf,sizeof(read_buf));
	printf("Read from driver: %s\n",read_buf);

	close(fd);
}
