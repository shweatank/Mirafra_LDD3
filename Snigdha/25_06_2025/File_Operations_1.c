#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>    
#include <unistd.h>    
#include <string.h>     


typedef struct 
{
    int (*open)(const char *filename, int flags, mode_t mode);
    void (*write)(int fd);
    void (*read)(int fd);
    void (*close)(int fd);
} FileOperations;


int my_open(const char *filename, int flags, mode_t mode) 
{
    int fd = open(filename, flags, mode);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    return fd;
}

void my_write(int fd) 
{
    char buffer[1024];
    ssize_t n;

    printf("Enter text (Ctrl+D to end):\n");
    while ((n = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) 
    {
        if (write(fd, buffer, n) == -1) 
	{
            perror("write");
            exit(2);
        }
    }
    printf("Writing Done\n");
}

void my_read(int fd) 
{
    char buffer[1024];
    ssize_t n;

    printf("\nReading from file:\n");
    while ((n = read(fd, buffer, sizeof(buffer))) > 0) 
    {
        if (write(STDOUT_FILENO, buffer, n) == -1) 
	{
            perror("write to stdout");
            exit(3);
        }
    }
}

void my_close(int fd) 
{
    if (close(fd) == -1) 
    {
        perror("close");
        exit(4);
    }
}

FileOperations fileOps = {
    .open = my_open,
    .write = my_write,
    .read = my_read,
    .close = my_close
};


int main() 
{
    int fd_write = fileOps.open("file1.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    fileOps.write(fd_write);
    fileOps.close(fd_write);


    int fd_read = fileOps.open("file1.txt", O_RDONLY, 0);
    fileOps.read(fd_read);
    fileOps.close(fd_read);

    return 0;
}

