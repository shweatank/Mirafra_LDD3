#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#define DEVICE "/dev/simple_char_dev"

enum 
{
	ADD=1,
	SUB,
	MUL,
	DIV
};

int main() 
{
    int fd;
    int input[3],res;
    char op;

    printf("Enter operation and integers(e.g:2*3):");
    scanf("%d %c %d",&input[0],&op,&input[1]);

    switch(op)
    {
	    case '+':input[2]=ADD;break;
	    case '-':input[2]=SUB;break;
	    case '*':input[2]=MUL;break;
	    case '/':input[2]=DIV;break;
	    default: 
		     printf("Invalid operation\n");
		     return 1;
    }

    fd = open(DEVICE, O_RDWR);
    if (fd < 0) 
    {
        perror("Failed to open device");
        return 1;
    }


    if (write(fd, input, sizeof(input)) < 0) 
    {
        perror("Write failed");
        close(fd);
        return 1;
    }

    lseek(fd, 0, SEEK_SET);

    int bytes = read(fd, &res, sizeof(res));
    if (bytes < 0) 
    {
        perror("Read failed");
        close(fd);
        return 1;
    }
    printf("Received result form kernel:%d\n",res);

    close(fd);
    return 0;
}

