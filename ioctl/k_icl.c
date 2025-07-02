// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>       // For module init/exit macros
#include <linux/module.h>     // Core header for loading LKMs into the kernel
#include <linux/fs.h>         // For register_chrdev and file_operations
#include <linux/uaccess.h>    // For copy_to_user and copy_from_user
#include <linux/ioctl.h>      // For _IOW and _IOR macros

// Define the name of the device node (e.g., /dev/k_icl)
#define DEVICE_NAME "k_icl"

// Define IOCTL command numbers
// _IOW: IOCTL to write data from user to kernel
// _IOR: IOCTL to read data from kernel to user
#define IOCTL_SET_NUM _IOW(100, 0, int)
#define IOCTL_GET_NUM _IOR(100, 1, int)

// Global variable to store data in kernel space
static int stored_val = 0;

// Store the dynamically allocated major number
static int major_num = 0;

/* ------------------------------------------------------------------------- */
/*                       File Operations Implementation                      */
/* ------------------------------------------------------------------------- */

// Called when the device file is opened
static int my_open(struct inode *inode, struct file *file) {
    pr_info("k_icl: device opened\n");
    return 0;  // success
}

// Called when the device file is closed
static int my_release(struct inode *inode, struct file *file) {
    pr_info("k_icl: device closed\n");
    return 0;  // success
}

// IOCTL system call handler
static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int tmp;

    switch (cmd) {
        case IOCTL_SET_NUM:
            // Copy integer value from user space to kernel space
            if (copy_from_user(&tmp, (int __user *)arg, sizeof(int)))
                return -EFAULT; // Return error if copy fails

            stored_val = tmp;  // Store received value
            pr_info("k_icl: Value set to %d\n", stored_val);
            break;

        case IOCTL_GET_NUM:
            // Copy stored value from kernel space to user space
            if (copy_to_user((int __user *)arg, &stored_val, sizeof(int)))
                return -EFAULT; // Return error if copy fails

            pr_info("k_icl: Value %d sent to user\n", stored_val);
            break;

        default:
            // Invalid command
            return -EINVAL;
    }

    return 0;  // success
}

/* ------------------------------------------------------------------------- */
/*                          File Operations Structure                        */
/* ------------------------------------------------------------------------- */

static struct file_operations fops = {
    .owner = THIS_MODULE,           // Prevents unloading while operations in progress
    .open = my_open,                // .open = open() system call
    .release = my_release,          // .release = close() system call
    .unlocked_ioctl = my_ioctl,     // .ioctl = ioctl() system call
};

/* ------------------------------------------------------------------------- */
/*                             Module Init & Exit                            */
/* ------------------------------------------------------------------------- */

// Called when the module is inserted using insmod
static int __init k_icl_init(void) {
    // Register the device and get a dynamic major number
    major_num = register_chrdev(0, DEVICE_NAME, &fops);

    if (major_num < 0) {
        pr_err("k_icl: Failed to register device\n");
        return major_num;
    }

    pr_info("k_icl: Module loaded with dynamic major %d\n", major_num);
    pr_info("Run: sudo mknod /dev/%s c %d 0\n", DEVICE_NAME, major_num);
    return 0;
}

// Called when the module is removed using rmmod
static void __exit k_icl_exit(void) {
    // Unregister the device using the stored major number
    unregister_chrdev(major_num, DEVICE_NAME);
    pr_info("k_icl: Module unloaded\n");
}

/* ------------------------------------------------------------------------- */
/*                           Register Module Hooks                           */
/* ------------------------------------------------------------------------- */

module_init(k_icl_init);   // Register init function
module_exit(k_icl_exit);   // Register exit function

/* ------------------------------------------------------------------------- */
/*                        Module Metadata for modinfo                        */
/* ------------------------------------------------------------------------- */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pritesh");
MODULE_DESCRIPTION("IOCTL demo with dynamic major number");

