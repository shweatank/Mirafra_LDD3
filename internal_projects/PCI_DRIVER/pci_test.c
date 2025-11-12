/*
 * pci_test.c - Test application for PCI driver
 * 
 * Compile: gcc -o pci_test pci_test.c
 * Usage: ./pci_test [read|write|info]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

#define DEVICE_PATH "/dev/pcidev"
#define BUFFER_SIZE 4096

void print_usage(const char *prog_name)
{
    printf("Usage: %s [command]\n", prog_name);
    printf("Commands:\n");
    printf("  read        - Read device information\n");
    printf("  write TEXT  - Write text to device\n");
    printf("  info        - Show device information\n");
    printf("  test        - Run comprehensive test\n");
    printf("\nExamples:\n");
    printf("  %s read\n", prog_name);
    printf("  %s write \"Hello PCI Device\"\n", prog_name);
    printf("  %s test\n", prog_name);
}

int test_read(int fd)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    
    printf("=== Reading from device ===\n");
    
    memset(buffer, 0, sizeof(buffer));
    bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    
    if (bytes_read < 0) {
        perror("Failed to read from device");
        return -1;
    }
    
    if (bytes_read == 0) {
        printf("No data available\n");
        return 0;
    }
    
    buffer[bytes_read] = '\0';
    printf("Read %zd bytes:\n%s\n", bytes_read, buffer);
    
    return 0;
}

int test_write(int fd, const char *data)
{
    ssize_t bytes_written;
    size_t data_len;
    
    printf("=== Writing to device ===\n");
    
    data_len = strlen(data);
    printf("Writing %zu bytes: \"%s\"\n", data_len, data);
    
    bytes_written = write(fd, data, data_len);
    
    if (bytes_written < 0) {
        perror("Failed to write to device");
        return -1;
    }
    
    printf("Successfully wrote %zd bytes\n", bytes_written);
    
    return 0;
}

int show_device_info(void)
{
    char buffer[256];
    FILE *fp;
    
    printf("=== Device Information ===\n");
    
    /* Check if device exists */
    if (access(DEVICE_PATH, F_OK) != 0) {
        printf("Device %s does not exist\n", DEVICE_PATH);
        return -1;
    }
    
    printf("Device path: %s\n", DEVICE_PATH);
    
    /* Show device permissions */
    snprintf(buffer, sizeof(buffer), "ls -l %s", DEVICE_PATH);
    printf("\nDevice permissions:\n");
    system(buffer);
    
    /* Show loaded modules */
    printf("\nLoaded PCI driver modules:\n");
    system("lsmod | grep pci_driver");
    
    /* Show PCI devices */
    printf("\nPCI Devices:\n");
    fp = popen("lspci | head -10", "r");
    if (fp) {
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            printf("  %s", buffer);
        }
        pclose(fp);
    }
    
    return 0;
}

int run_comprehensive_test(void)
{
    int fd;
    int ret = 0;
    const char *test_data = "PCI Driver Test Data";
    
    printf("=== Comprehensive PCI Driver Test ===\n\n");
    
    /* Show device info */
    show_device_info();
    printf("\n");
    
    /* Open device */
    printf("=== Opening device %s ===\n", DEVICE_PATH);
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        printf("Make sure:\n");
        printf("  1. The driver is loaded (sudo insmod pci_driver.ko)\n");
        printf("  2. The device node exists (ls -l %s)\n", DEVICE_PATH);
        printf("  3. You have permission to access the device\n");
        return -1;
    }
    printf("Device opened successfully (fd=%d)\n\n", fd);
    
    /* Test read */
    if (test_read(fd) < 0) {
        ret = -1;
        goto cleanup;
    }
    printf("\n");
    
    /* Test write */
    if (test_write(fd, test_data) < 0) {
        ret = -1;
        goto cleanup;
    }
    printf("\n");
    
    /* Test multiple small writes */
    printf("=== Testing multiple writes ===\n");
    for (int i = 0; i < 3; i++) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Test iteration %d", i);
        printf("Write %d: ", i + 1);
        if (write(fd, buf, strlen(buf)) < 0) {
            perror("write failed");
            ret = -1;
            goto cleanup;
        }
        printf("OK\n");
    }
    printf("\n");
    
    /* Reset file position and read again */
    lseek(fd, 0, SEEK_SET);
    printf("=== Reading after write ===\n");
    if (test_read(fd) < 0) {
        ret = -1;
        goto cleanup;
    }
    
    printf("\n=== All tests completed successfully ===\n");

cleanup:
    close(fd);
    return ret;
}

int main(int argc, char *argv[])
{
    int fd;
    int ret = 0;
    
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    if (strcmp(argv[1], "info") == 0) {
        return show_device_info();
    }
    
    if (strcmp(argv[1], "test") == 0) {
        return run_comprehensive_test();
    }
    
    /* Open device for read/write commands */
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        printf("\nTroubleshooting:\n");
        printf("  1. Check if driver is loaded: lsmod | grep pci_driver\n");
        printf("  2. Check device node: ls -l %s\n", DEVICE_PATH);
        printf("  3. Load driver: sudo insmod pci_driver.ko\n");
        return 1;
    }
    
    if (strcmp(argv[1], "read") == 0) {
        ret = test_read(fd);
    }
    else if (strcmp(argv[1], "write") == 0) {
        if (argc < 3) {
            printf("Error: write command requires data argument\n");
            print_usage(argv[0]);
            ret = 1;
        } else {
            ret = test_write(fd, argv[2]);
        }
    }
    else {
        printf("Unknown command: %s\n", argv[1]);
        print_usage(argv[0]);
        ret = 1;
    }
    
    close(fd);
    return ret;
}
