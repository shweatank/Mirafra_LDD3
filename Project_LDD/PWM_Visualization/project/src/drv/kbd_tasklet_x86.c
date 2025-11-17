#include <linux/module.h>       // Core header for loading LKMs into the kernel
#include <linux/init.h>         // Macros used to mark up functions e.g., __init __exit
#include <linux/interrupt.h>    // For IRQ request/free APIs
#include <linux/fs.h>           // For character device registration
#include <linux/uaccess.h>      // For copy_to_user, copy_from_user
#include <linux/device.h>       // For device creation
#include <linux/slab.h>         // For kmalloc, kfree
#include <linux/mutex.h>        // For mutex
#include <asm/io.h>             // For inb()

#define KBD_IRQ        1                  // Keyboard IRQ number
#define DEVICE_NAME    "characters_pressed" // Device file name in /dev
#define CLASS_NAME     "kbd_class"        // Device class name
#define MAX_BUFFER     128                // Maximum buffer size for storing keys

static char key_buffer[MAX_BUFFER];       // Buffer to store pressed keys
static int buffer_index = 0;              // Current position in buffer
static unsigned char last_scancode;       // Last scancode read from keyboard

static int major;                         // Major number for char device
static struct class *kbd_class = NULL;    // Device class pointer
static struct device *kbd_device = NULL;  // Device structure pointer

static DEFINE_MUTEX(kbd_mutex);           // Mutex for buffer access synchronization

// Scancode to character map for digits 1 to 0
static const char keymap[128] = {
    [0x02] = '1',
    [0x03] = '2',
    [0x04] = '3',
    [0x05] = '4',
    [0x06] = '5',
    [0x07] = '6',
    [0x08] = '7',
    [0x09] = '8',
    [0x0A] = '9',
    [0x0B] = '0'
};

// Define tasklet structure
static struct tasklet_struct keyboard_tasklet;

// Tasklet function to process scancode in bottom half
static void tasklet_fn(struct tasklet_struct *t)
{
    if (last_scancode & 0x80)
        return;  // Ignore key release events

    mutex_lock(&kbd_mutex);

    if (last_scancode == 0x1C) {  // Check for Enter key
        printk(KERN_INFO "Enter pressed. Final buffer: %s\n", key_buffer);
        mutex_unlock(&kbd_mutex);
        return;
    }

    // If valid key in keymap, add to buffer
    if (last_scancode < 128 && keymap[last_scancode] != 0) {
        if (buffer_index < MAX_BUFFER - 1) {
            key_buffer[buffer_index++] = keymap[last_scancode];
            key_buffer[buffer_index] = '\0'; // Null terminate buffer
            printk(KERN_INFO "Key logged: %c\n", keymap[last_scancode]);
        }
    } else {
        printk(KERN_INFO "Invalid key pressed. Scancode: 0x%x\n", last_scancode);
    }

    mutex_unlock(&kbd_mutex);
}

// IRQ handler (top-half) – quickly reads scancode and schedules tasklet
static irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    last_scancode = inb(0x60);             // Read scancode from keyboard I/O port
    tasklet_schedule(&keyboard_tasklet);   // Schedule tasklet (bottom half)
    return IRQ_HANDLED;
}

// Character device read function
static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset)
{
    ssize_t bytes_read = 0;

    mutex_lock(&kbd_mutex);

    if (*offset >= buffer_index) {
        mutex_unlock(&kbd_mutex);
        return 0;  // No more data to read
    }

    // Read data from buffer to user space
    bytes_read = simple_read_from_buffer(buffer, len, offset, key_buffer, buffer_index);

    mutex_unlock(&kbd_mutex);
    return bytes_read;
}

// File operations structure
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read  = dev_read,
};

// Module init function
static int __init keyboard_logger_init(void)
{
    int ret;

    mutex_init(&kbd_mutex);  // Initialize mutex

    // Request shared IRQ line for keyboard
    ret = request_irq(KBD_IRQ, keyboard_irq_handler, IRQF_SHARED, "keyboard_logger_irq", (void *)(keyboard_irq_handler));
    if (ret) {
        printk(KERN_ERR "Failed to request IRQ\n");
        return ret;
    }

    // Initialize tasklet with function and dummy data (new API doesn't need data)
    tasklet_init(&keyboard_tasklet, (void *)tasklet_fn, 0);

    // Register character device and get major number
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ERR "Failed to register char device\n");
        free_irq(KBD_IRQ, (void *)(keyboard_irq_handler));
        return major;
    }

    // Create device class
    kbd_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(kbd_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        free_irq(KBD_IRQ, (void *)(keyboard_irq_handler));
        return PTR_ERR(kbd_class);
    }

    // Create device node in /dev
    kbd_device = device_create(kbd_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(kbd_device)) {
        class_destroy(kbd_class);
        unregister_chrdev(major, DEVICE_NAME);
        free_irq(KBD_IRQ, (void *)(keyboard_irq_handler));
        return PTR_ERR(kbd_device);
    }

    printk(KERN_INFO "Keyboard logger with tasklet loaded. Read from /dev/%s\n", DEVICE_NAME);
    return 0;
}

// Module exit function
static void __exit keyboard_logger_exit(void)
{
    tasklet_kill(&keyboard_tasklet);                         // Kill tasklet
    device_destroy(kbd_class, MKDEV(major, 0));              // Destroy device node
    class_destroy(kbd_class);                                // Destroy class
    unregister_chrdev(major, DEVICE_NAME);                   // Unregister char device
    free_irq(KBD_IRQ, (void *)(keyboard_irq_handler));       // Free IRQ
    printk(KERN_INFO "Keyboard logger unloaded\n");
}

// Register init and exit functions
module_init(keyboard_logger_init);
module_exit(keyboard_logger_exit);

// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Team 2");
MODULE_DESCRIPTION("Keyboard interrupt driver which takes keys from 1 to 0");

