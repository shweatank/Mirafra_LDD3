// Required Linux kernel headers for module development
#include <linux/init.h>              // Macros like module_init and module_exit
#include <linux/module.h>            // Needed by all kernel modules
#include <linux/fs.h>                // File operations structure
#include <linux/uaccess.h>           // For copy_to_user, copy_from_user
#include <linux/interrupt.h>         // For handling hardware interrupts
#include <linux/wait.h>              // Wait queues
#include <linux/io.h>                // For inb() - port I/O
#include <linux/device.h>            // For class and device creation
#include <linux/workqueue.h>         // For deferring work (workqueues)

#define DEVICE_NAME "kbdchar"        // Name for the character device
#define KBD_DATA_PORT 0x60           // I/O port address for keyboard data
#define KEYBOARD_IRQ 1               // IRQ number for standard PS/2 keyboard

//#define MAGIC 'a'                    // Magic number for IOCTL (not used here)



// Module metadata
MODULE_LICENSE("GPL");                      // License of the module
MODULE_AUTHOR("Group1");                       // Author name
MODULE_DESCRIPTION("Keyboard IRQ + Workqueue + IOCTL module"); // Description

static int major;                           // Major number assigned to device
static struct class *kbd_class;             // Pointer to class for auto-creating device file

static int speed = 0, max_speed = 0;        // Not used currently, was for IOCTL
static unsigned char scancode;              // Stores last valid scancode
static int data_available = 0;              // Flag to indicate if new data available

static DECLARE_WAIT_QUEUE_HEAD(wq);         // Wait queue for blocking read()
static struct work_struct sc_work;          // Work structure for deferred work

// Workqueue function, runs in process context after interrupt
static void sc_work_func(struct work_struct *work)
{
    data_available = 1;                     // Set data_available flag
    wake_up_interruptible(&wq);            // Wake up any process waiting on wait queue
}

// Interrupt handler for keyboard
static irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char sc = inb(KBD_DATA_PORT); // Read scancode from port 0x60
    if (sc == 0xE0 || sc & 0x80)           // Ignore extended key or key release
        return IRQ_HANDLED;

    scancode = sc;                         // Store valid scancode
    schedule_work(&sc_work);              // Defer work to bottom half
    return IRQ_HANDLED;
}

// File operation: read - returns one scancode to userspace
static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
    wait_event_interruptible(wq, data_available); // Sleep until data available
    data_available = 0;                            // Clear flag

    if (copy_to_user(buf, &scancode, 1))           // Copy 1 byte (scancode) to userspace
        return -EFAULT;

    return 1;                                      // Return number of bytes read
}


// File operations structure
static struct file_operations fops = {
    .owner = THIS_MODULE,         // Owner of file ops
    .read = dev_read,             // Read handler
    //.unlocked_ioctl = dev_ioctl,  // IOCTL handler (commented)
};

// Module initialization function
static int __init kbd_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops); // Register char device with dynamic major
    if (major < 0) {
        pr_err("Failed to register char device\n");
        return major;
    }

    kbd_class = class_create(DEVICE_NAME);          // Create device class
    if (IS_ERR(kbd_class)) {                        // Check for error
        unregister_chrdev(major, DEVICE_NAME);      // Rollback
        return PTR_ERR(kbd_class);
    }

    device_create(kbd_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME); // Create /dev/kbdchar

    INIT_WORK(&sc_work, sc_work_func);              // Initialize workqueue with handler

    if (request_irq(KEYBOARD_IRQ, keyboard_irq_handler, IRQF_SHARED,
                    "kbd_irq", (void *)&kbd_init))  // Request IRQ 1 for keyboard
    {
        pr_err("IRQ request failed\n");             // Error handling
        device_destroy(kbd_class, MKDEV(major, 0));
        class_destroy(kbd_class);
        unregister_chrdev(major, DEVICE_NAME);
        return -EBUSY;
    }

    pr_info("kbdchar: Module loaded. Press a key...\n"); // Log info
    return 0;
}

// Module cleanup function
static void __exit kbd_exit(void)
{
    flush_work(&sc_work);                           // Ensure pending work is completed
    free_irq(KEYBOARD_IRQ, (void *)&kbd_init);      // Free IRQ
    device_destroy(kbd_class, MKDEV(major, 0));     // Destroy device node
    class_destroy(kbd_class);                       // Destroy class
    unregister_chrdev(major, DEVICE_NAME);          // Unregister char device
    pr_info("kbdchar: Module unloaded.\n");         // Log info
}

// Declare init and exit functions
module_init(kbd_init);
module_exit(kbd_exit);

