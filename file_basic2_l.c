#include <stdio.h>
#include <fcntl.h>     
#include <unistd.h>    
#include <string.h>    
#include <stdlib.h>     

int main() {
    int fd;
    char *filename = "temp.txt";
    char *text = "Hello, this is written using write() system call.\n";
    char buffer[100];
    int bytesRead;

    fd = open(filename, O_WRONLY | O_CREAT ,0644);
    if (fd < 0) {
        perror("Error opening file for writing");
        exit(1);
    }

    write(fd, text, strlen(text));
    close(fd);

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Error opening file for reading");
        exit(1);
    }

    printf("Reading from the file:\n");
    while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0'; 
      	printf("%s", buffer);
    }

    close(fd);
    return 0;
}

