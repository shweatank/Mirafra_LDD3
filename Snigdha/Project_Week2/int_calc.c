#include <linux/module.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/ctype.h>
#include<asm/io.h>

#include "int_calc.h"

#define DEVICE_NAME "int_calc"
#define IRQ_NUM 1  // Keyboard IRQ

static int major;
static bool logging = false;
static char buffer[32];
static int buf_index = 0;

static int operand1 = 0, operand2 = 0;
static char operator = 0;
static int result = 0;

static irqreturn_t irq_handler(int irq, void *dev_id) 
{
    unsigned char scancode = inb(0x60);
    unsigned char keycode = scancode & 0x7F;

    // Basic mapping for digits and operators from scan codes
    char ascii_map[128] = {
        [2] = '1', [3] = '2', [4] = '3', [5] = '4', [6] = '5', [7] = '6',
        [8] = '7', [9] = '8', [10] = '9', [11] = '0',
        [12] = '-', [13] = '=', [28] = '\n',
        [74] = '-', [78] = '+', [55] = '*', [53] = '/'
    };

    if (logging && !(scancode & 0x80)) {  // key press only
        char key = ascii_map[keycode];
        if (key && buf_index < sizeof(buffer) - 1) {
            buffer[buf_index++] = key;
            buffer[buf_index] = '\0';
        }
    }

    return IRQ_HANDLED;
}

static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case IOCTL_START_INPUT:
            logging = true;
            buf_index = 0;
            memset(buffer, 0, sizeof(buffer));
            break;

        case IOCTL_STOP_INPUT:
            logging = false;
	    printk(KERN_INFO "Captured buffer: '%s'\n", buffer);

            sscanf(buffer, "%d%c%d", &operand1, &operator, &operand2);
            switch (operator) {
                case '+': result = operand1 + operand2; break;
                case '-': result = operand1 - operand2; break;
                case '*': result = operand1 * operand2; break;
                case '/': result = (operand2 != 0) ? operand1 / operand2 : 0; break;
                default: result = 0;
            }
            break;

        case IOCTL_GET_RESULT:
            if (copy_to_user((int __user *)arg, &result, sizeof(result)))
                return -EFAULT;
            break;

        default:
            return -EINVAL;
    }
    return 0;
}

static int device_open(struct inode *inode, struct file *file) {
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = device_ioctl,
    .open = device_open,
    .release = device_release,
};

static int __init calc_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) return major;

    if (request_irq(IRQ_NUM, irq_handler, IRQF_SHARED, DEVICE_NAME, (void *)&irq_handler)) {
        unregister_chrdev(major, DEVICE_NAME);
        return -EBUSY;
    }

    printk(KERN_INFO "int_calc loaded with major %d\n", major);
    return 0;
}

static void __exit calc_exit(void) {
    free_irq(IRQ_NUM, (void *)&irq_handler);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "int_calc unloaded\n");
}

module_init(calc_init);
module_exit(calc_exit);

MODULE_LICENSE("GPL");

