#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int choice;
    int fd;
    
    while(1) {
        printf("\n1. OFF\n2. Slow\n3. Fast\n4. Exit\nChoice: ");
        scanf("%d", &choice);
        
        fd = open("/sys/kernel/led/pattern", O_WRONLY);
        
        switch(choice) {
            case 1: write(fd, "0", 1); break;
            case 2: write(fd, "1", 1); break;
            case 3: write(fd, "2", 1); break;
            case 4: write(fd, "0", 1); close(fd); exit(0);
        }
        
        close(fd);
    }
    
    return 0;
}
