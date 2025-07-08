#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <asm/io.h>

#define DEVICE_NAME "bitwise_log"
#define MAJOR_NUM 150
#define BITWISE_IOC_MAGIC 0xF6
#define BITWISE_GET_STATUS _IOR(BITWISE_IOC_MAGIC, 0, uint8_t)
#define BITWISE_RESET_STATUS _IO(BITWISE_IOC_MAGIC, 1)
#define BITWISE_SET_DATA _IOW(BITWISE_IOC_MAGIC, 2, uint8_t)

static uint8_t bit_data = 0;
static bool initialized = false;

static int key_to_bit(char key) {
    if (key >= 'a' && key <= 'h')
        return key - 'a';
    return -1;
}

static void toggle_bit_by_key(char key) {
    if (!initialized) {
        printk(KERN_WARNING "bitwise_logger: Please enter bit data first before toggling (key: '%c')\n", key);
        return;
    }

    int bit = key_to_bit(key);
    if (bit >= 0 && bit < 8) {
        bit_data ^= (1 << bit);
        printk(KERN_INFO "bitwise_logger: Toggled bit %d by key '%c', new bit_data = 0x%02x\n",
               bit, key, bit_data);
    }
}

irqreturn_t keyboard_irq_handler(int irq, void *dev_id) {
    unsigned char scancode = inb(0x60);
    char key = 0;

    switch (scancode & 0x7F) {
        case 0x1E: key = 'a'; break;
        case 0x30: key = 'b'; break;
        case 0x2E: key = 'c'; break;
        case 0x20: key = 'd'; break;
        case 0x12: key = 'e'; break;
        case 0x21: key = 'f'; break;
        case 0x22: key = 'g'; break;
        case 0x23: key = 'h'; break;
        default: key = 0; break;
    }

    if (!(scancode & 0x80) && key) {
        toggle_bit_by_key(key);
    }

    return IRQ_HANDLED;
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    uint8_t data;
    switch (cmd) {
        case BITWISE_GET_STATUS:
            if (copy_to_user((uint8_t __user *)arg, &bit_data, sizeof(bit_data)))
                return -EFAULT;
            break;

        case BITWISE_RESET_STATUS:
            bit_data = 0;
            initialized = false;
            printk(KERN_INFO "bitwise_logger: Bit data reset. Please enter bit data again.\n");
            break;

        case BITWISE_SET_DATA:
            if (copy_from_user(&data, (uint8_t __user *)arg, sizeof(data)))
                return -EFAULT;
            bit_data = data;
            initialized = true;
            printk(KERN_INFO "bitwise_logger: Bit data set to 0x%02x\n", bit_data);
            break;

        default:
            return -EINVAL;
    }
    return 0;
}

static int my_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "bitwise_logger: Device opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "bitwise_logger: Device closed\n");
    return 0;
}

static const struct file_operations fops = {
    .unlocked_ioctl = my_ioctl,
    .open = my_open,
    .release = my_release,
};

static int __init bitwise_init(void) {
    int ret;
    ret = request_irq(1, keyboard_irq_handler, IRQF_SHARED, "keyboard_irq_handler", (void *)keyboard_irq_handler);
    if (ret) {
        printk(KERN_ERR "bitwise_logger: Failed to request IRQ\n");
        return ret;
    }

    ret = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
    if (ret < 0) {
        free_irq(1, (void *)keyboard_irq_handler);
        printk(KERN_ERR "bitwise_logger: Failed to register char device\n");
        return ret;
    }

    printk(KERN_INFO "bitwise_logger: Registered with major %d\n", MAJOR_NUM);
    return 0;
}

static void __exit bitwise_exit(void) {
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    free_irq(1, (void *)keyboard_irq_handler);
    printk(KERN_INFO "bitwise_logger: Unregistered device\n");
}

module_init(bitwise_init);
module_exit(bitwise_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("swarna");
MODULE_DESCRIPTION("Bitwise toggle kernel module that asks user to enter bit data first");

