//If i press 'A' then keyboard interuupt will triggers key is sent to userspace then a string is sent to kernel space through(write() and default action takes place(reversing) ans sent to user space through (read())
//if 'B' is pressed then a string is sent through IOCTL and ask choice for operation and after particular operation ioctl read and print the string which taken from kernel space.
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE_NAME "/dev/myproject"
#define MAGIC       'a'

struct st {
    char str2[50];
    int  choice;
    int  len;
}x;

#define IOCTL_OP _IOWR(MAGIC, 0, struct st)

#define KEY_A 0x1E
#define KEY_B 0x30

int main(void)
{
    int fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        perror("open " DEVICE_NAME);
        return 1;
    }

    printf("Waiting for keyboard events …\n"
           "  Press ‘A’ for Default(reversing).\n" //through write and read
           "  Press ‘B’ for string operations.\n"); //through ioctl

    while(1) {
    
        unsigned char sc;
        char str1[50];
        

        if (read(fd, &sc, 1) != 1)
            continue;
        if (sc & 0x80)            //ignore key‑release codes
            continue;

        
        if (sc == KEY_A) { //done by normal read and write of character device driver.
            printf("Enter string to send to kernel: ");
            scanf(" %[^\n]", str1);

            if (write(fd, str1, strlen(str1) + 1) < 0)
                perror("write");

            lseek(fd, 0, SEEK_SET);

            if (read(fd, str1, sizeof str1) > 0)
                printf("kernel returned \"%s\"\n", str1);
            else
                perror("read");
        }
        
        else if (sc == KEY_B) {  
         //done by ioctl read and write
            printf("Enter string to send to kernel: ");
            scanf(" %[^\n]", x.str2);

            printf("Enter choice 1)length  2)toggle‑case: ");
            if (scanf("%d", &x.choice) != 1)
                continue;

            if (ioctl(fd, IOCTL_OP, &x) == 0) {
                if (x.choice == 1)
                    printf("kernel returned length = %d\n", x.len);
                else
                    printf("kernel returned \"%s\"\n", x.str2);
            } else {
                perror("ioctl");
            }
        }
    }
}

