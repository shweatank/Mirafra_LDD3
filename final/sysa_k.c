// System A: Kernel Module using waitqueue without IOCTL

#include <linux/module.h>          // Core header for loading LKMs into the kernel
#include <linux/init.h>            // Macros for module init and exit functions
#include <linux/fs.h>              // File operations structure
#include <linux/device.h>          // Device creation and class support
#include <linux/uaccess.h>         // For copy_to_user(), copy_from_user()
#include <linux/interrupt.h>       // For request_irq(), free_irq()
#include <linux/keyboard.h>        // Keyboard IRQ constants
#include <linux/wait.h>            // Waitqueue support
#include <linux/sched.h>           // For TASK_INTERRUPTIBLE
#include <linux/poll.h>            // For poll support (not used here but good practice)
#include <asm/io.h>                // For inb(), reading from I/O ports

#define DEVICE_NAME "kbd_irq_dev"  // Name of the character device
#define KBD_IRQ 1                  // IRQ number for the keyboard (IRQ 1 on x86)

static int major;                  // Stores dynamically assigned major number
static struct class *cls;          // Device class pointer
static int dev_id;                 // Unique ID used when requesting the IRQ

// Declare a waitqueue named 'waitq'
static DECLARE_WAIT_QUEUE_HEAD(waitq);  

// Flag to indicate whether the required key has been pressed
static int irq_flag = 0;           // Set in IRQ handler, cleared in read()

// Interrupt handler for the keyboard IRQ
static irqreturn_t irq_handler(int irq, void *dev_id_struct) {
    // Read the scancode from the keyboard data port (I/O port 0x60)
    unsigned char scancode = inb(0x60);

    // Log the received scancode
    pr_info(KERN_INFO "received scancode:0x%x\n", scancode);

    // Check if the scancode corresponds to the 's' key (0x1F)
    if (scancode == 0x1F) {
        pr_info(KERN_INFO "received 's' scancode\n");

        // Set the flag indicating the key was pressed
        irq_flag = 1;

        // Wake up any process waiting on the waitqueue
        wake_up_interruptible(&waitq);
    }

    // Inform the kernel that the interrupt was handled
    return IRQ_HANDLED;
}

// Read function called when user does read() on the device
static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    int ret;

    // Log that the read function was called and process is waiting
    pr_info("read() called, waiting for key press...\n");

    // Block the process until irq_flag becomes non-zero
    ret = wait_event_interruptible(waitq, irq_flag != 0);
    if (ret)
        return -ERESTARTSYS;  // Interrupted by signal

    // Log after key press unblocks the read
    pr_info("read() unblocked, key was pressed\n");

    // Reset the irq_flag
    irq_flag = 0;

    // Optionally send dummy data to userspace to indicate success
    if (buf != NULL && len > 0) {
        char msg = 'K'; // Dummy character
        copy_to_user(buf, &msg, 1);
        return 1;
    }

    return len;  // Return number of bytes read
}

// File operations structure linking system calls to our handlers
static struct file_operations fops = {
    .read = dev_read  // Only read() is implemented
};

// Module initialization function (called when insmod is done)
static int __init irq_mod_init(void) {
    // Register character device and get major number
    major = register_chrdev(0, DEVICE_NAME, &fops);

    // Create device class
    cls = class_create(DEVICE_NAME);

    // Create the device file under /dev
    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    // Request keyboard IRQ (IRQ 1), with shared mode
    if (request_irq(KBD_IRQ, irq_handler, IRQF_SHARED, DEVICE_NAME, &dev_id)) {
        pr_err("Failed to request IRQ\n");
        return -1;
    }

    pr_info("Keyboard IRQ module loaded.\n");
    return 0;  // Success
}

// Module cleanup function (called when rmmod is done)
static void __exit irq_mod_exit(void) {
    // Free the keyboard IRQ
    free_irq(KBD_IRQ, &dev_id);

    // Destroy device and class
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);

    // Unregister the character device
    unregister_chrdev(major, DEVICE_NAME);

    pr_info("Keyboard IRQ module unloaded.\n");
}

// Register init and exit functions with the kernel
module_init(irq_mod_init);
module_exit(irq_mod_exit);

// License declaration
MODULE_LICENSE("GPL");

