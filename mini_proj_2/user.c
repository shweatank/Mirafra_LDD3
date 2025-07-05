#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<stdint.h>

#define MAGIC_NUM 'K'
#define DEVICE_NAME "/dev/log"
#define LOG_SIZE 10

#define KEY_DISABLE _IOW(MAGIC_NUM,1,int)
#define KEY_ENABLE _IOW(MAGIC_NUM,2,int)
#define KEY_LOG _IOR(MAGIC_NUM,3,char *)

const char scancode_to_ascii[128] = {
    [0x02] = '1', [0x03] = '2', [0x04] = '3', [0x05] = '4',
    [0x06] = '5', [0x07] = '6', [0x08] = '7', [0x09] = '8',
    [0x0A] = '9', [0x0B] = '0',
    [0x10] = 'q', [0x11] = 'w', [0x12] = 'e', [0x13] = 'r',
    [0x14] = 't', [0x15] = 'y', [0x16] = 'u', [0x17] = 'i',
    [0x18] = 'o', [0x19] = 'p',
    [0x1E] = 'a', [0x1F] = 's', [0x20] = 'd', [0x21] = 'f',
    [0x22] = 'g', [0x23] = 'h', [0x24] = 'j', [0x25] = 'k',
    [0x26] = 'l',
    [0x2C] = 'z', [0x2D] = 'x', [0x2E] = 'c', [0x2F] = 'v',
    [0x30] = 'b', [0x31] = 'n', [0x32] = 'm',
    [0x39] = ' ',  // space
    [0x1C] = '\n', // enter
};

int main()
{
	int fd,key;
	char log[10];
	fd=open(DEVICE_NAME,O_RDWR);
	if(fd<0)
	{
		perror("message\n");
		exit(0);
	}
	printf("Enabling the key logger\n");
	key=1;
	ioctl(fd,KEY_ENABLE,&key);
	printf(".......waiting for keys.......\n");
	sleep(10);
	printf("Requesting the kernel to the key logging\n");
	ioctl(fd,KEY_LOG,log);
	printf("Displaying the logging info......\n");
	printf("Scancodes (hex): ");
        for (int i = 0; i < LOG_SIZE; i++) {
         uint8_t code = log[i];

        if (code < 0x80) { // Only handle key press (not release)
        char ch = scancode_to_ascii[code];
        if (ch)
            printf("%c", ch);
        else
            printf("[0x%02x]", code);  // Unknown key
        }
        }
        printf("\n");

	printf("Disabling the kernel logging....\n");
	key=0;
	ioctl(fd,KEY_DISABLE,&key);
}


