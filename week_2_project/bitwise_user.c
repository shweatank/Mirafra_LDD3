#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE_FILE "/dev/bitwise_log"
#define BITWISE_IOC_MAGIC 0xF6
#define BITWISE_GET_STATUS _IOR(BITWISE_IOC_MAGIC, 0, uint8_t)
#define BITWISE_RESET_STATUS _IO(BITWISE_IOC_MAGIC, 1)
#define BITWISE_SET_DATA _IOW(BITWISE_IOC_MAGIC, 2, uint8_t)

void print_menu()
{
    printf("\nBitwise Logger User Interface:\n");
    printf("1. Set Bit Data (in hex) [Required before toggling]\n");
    printf("2. Get Bit Status\n");
    printf("3. Reset Bit Status\n");
    printf("4. Exit\n");
    printf("Choose an option: ");
}

int main() {
    int fd;
    uint8_t bit_data;
    int choice;

    fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device file");
        return EXIT_FAILURE;
    }

    while (1)
    {
        print_menu();
        if (scanf("%d", &choice) != 1) 
	{
            fprintf(stderr, "Invalid input. Exiting.\n");
            break;
        }
            case 1:
                printf("Enter hex value (e.g., 0x5A): ");
                if (scanf("%hhx", &bit_data) != 1) {
                    fprintf(stderr, "Invalid hex value.\n");
                    // clear invalid input
                    while (getchar() != '\n');
                    break;
                }
                if (ioctl(fd, BITWISE_SET_DATA, &bit_data) == -1) {
                    perror("ioctl: SET_DATA failed");
                } else {
                    printf("Bit data initialized to 0x%02X\n", bit_data);
                }
                break;

            case 2:
                if (ioctl(fd, BITWISE_GET_STATUS, &bit_data) == -1) {
                    perror("ioctl: GET_STATUS failed");
                } else {
                    printf("Current Bit Data: 0x%02X\n", bit_data);
                }
                break;

            case 3:
                if (ioctl(fd, BITWISE_RESET_STATUS) == -1) {
                    perror("ioctl: RESET_STATUS failed");
                } else {
                    printf("Bit data has been reset. Please set it again before using keyboard toggles.\n");
                }
                break;

            case 4:
                close(fd);
                printf("Exiting.\n");
                return EXIT_SUCCESS;

            default:
                printf("Invalid option. Try again.\n");
        }
    }

    close(fd);
    return EXIT_FAILURE;
}

