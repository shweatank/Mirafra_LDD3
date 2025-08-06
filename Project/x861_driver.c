#include <linux/module.h>          // Needed by all modules
#include <linux/init.h>            // For module init/exit macros
#include <linux/fs.h>              // For character device registration (file_operations)
#include <linux/uaccess.h>         // For copy_from_user()
#include <linux/cdev.h>            // For cdev struct (not used directly here)
#include <linux/ioctl.h>           // For ioctl definitions
#include <linux/tty.h>             // For TTY-related structs (general terminal handling)
#include <linux/serial_core.h>     // For serial port core operations

// Device name shown under /dev
#define DEVICE_NAME "x86_uart"

// IOCTL command macros for sending password and threshold
#define IOCTL_SEND_PASSWORD    _IOW('x', 1, char *) // Write-only, sends char * from user to kernel
#define IOCTL_SEND_THRESHOLD   _IOW('x', 2, int)    // Write-only, sends int from user to kernel

static int major;                      // Major number assigned to the device
static struct file *uart_filp;         // File pointer for UART device (/dev/ttyUSB0)
static char stored_password[32];       // Buffer to store received password from user-space
static int stored_threshold;           // Integer to store threshold value

// IOCTL handler - processes IOCTL calls from user-space
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    char msg[128];  // Buffer to hold formatted message before sending via UART

    switch (cmd) {
        case IOCTL_SEND_PASSWORD:
            // Copy password from user space to kernel space
            if (copy_from_user(stored_password, (char __user *)arg, sizeof(stored_password)))
                return -EFAULT; // Return error if copy fails

            printk(KERN_INFO "[x86_uart] Stored password updated\n");
            break;

        case IOCTL_SEND_THRESHOLD:
            // Check if UART file is opened correctly
            if (!uart_filp || IS_ERR(uart_filp)) {
                printk(KERN_ERR "[x86_uart] UART device not opened\n");
                return -ENODEV;
            }

            // Copy threshold value from user space
            if (copy_from_user(&stored_threshold, (int __user *)arg, sizeof(int)))
                return -EFAULT;

            // Format a message to be sent over UART
            snprintf(msg, sizeof(msg), "PASS:%s THRESH:%d\n", stored_password, stored_threshold);

            uart_filp->f_pos = 0; // Reset file pointer before writing

            // Send message to UART device
            if (kernel_write(uart_filp, msg, strlen(msg), &uart_filp->f_pos) < 0) {
                printk(KERN_ERR "[x86_uart] Failed to write to UART\n");
                return -EIO;
            }

            printk(KERN_INFO "[x86_uart] Sent config over UART: %s", msg);
            break;

        default:
            return -EINVAL;  // Invalid IOCTL command
    }
    return 0;  // Success
}

// Called when user opens /dev/x86_uart
static int dev_open(struct inode *inode, struct file *file) {
    // Open UART device file for read/write, no controlling terminal, synchronous I/O
    uart_filp = filp_open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_SYNC, 0);
    if (IS_ERR(uart_filp)) {
        printk(KERN_ERR "[x86_uart] Cannot open /dev/ttyUSB0\n");
        return PTR_ERR(uart_filp);  // Return negative error code
    }

    printk(KERN_INFO "[x86_uart] UART device opened successfully\n");
    return 0;
}

// Called when user closes /dev/x86_uart
static int dev_release(struct inode *inode, struct file *file) {
    if (uart_filp && !IS_ERR(uart_filp)) {
        filp_close(uart_filp, NULL);  // Close UART file
        uart_filp = NULL;
        printk(KERN_INFO "[x86_uart] UART device closed\n");
    }
    return 0;
}

// File operations structure - maps system calls to driver functions
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_release,
    .unlocked_ioctl = dev_ioctl,  // For handling IOCTL commands
};

// Module initialization
static int __init x86_uart_init(void) {
    // Register character device dynamically, return assigned major number
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ERR "[x86_uart] Failed to register character device\n");
        return major;
    }

    // Log message with mknod command for user-space to create device file
    printk(KERN_INFO "[x86_uart] Loaded: mknod /dev/%s c %d 0\n", DEVICE_NAME, major);
    return 0;
}

// Module cleanup
static void __exit x86_uart_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);  // Unregister char device

    // If UART file was open, close it
    if (uart_filp && !IS_ERR(uart_filp)) {
        filp_close(uart_filp, NULL);
    }

    printk(KERN_INFO "[x86_uart] Unloaded\n");
}

// Register module entry and exit points
module_init(x86_uart_init);
module_exit(x86_uart_exit);

// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("TEAM3");
MODULE_DESCRIPTION("X86 UART Driver");
