#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

struct calc_data {
    int a;
    int b;
    char op;
    int result;
}d;

int main()
{
    int fd = open("/dev/mywait", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    printf("Enter the data\n");
   scanf("%d %c %d",&d.a,&d.op,&d.b); 

    write(fd, &d, sizeof(d));  // Send data to kernel

    close(fd);
    return 0;
}

