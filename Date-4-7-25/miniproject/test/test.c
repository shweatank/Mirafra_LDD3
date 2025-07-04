#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/workqueue.h>
#include <asm/io.h>

#define DEVICE "ioctl_demo"
#define MAJOR_NUM 100
#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60

#define IOCTL_SET_VALUE _IOW(MAJOR_NUM, 0, int)
#define IOCTL_GET_VALUE _IOR(MAJOR_NUM, 1, int)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Harsha");
MODULE_DESCRIPTION("Keyboard IRQ Calculator with IOCTL and Workqueue");

struct cal {
    int a;
    int b;
    int flag;
} c;

int res = 0;
int shift_pressed = 0;
int last_scancode = 0;
static struct work_struct mywork;
static void function(struct work_struct *work)
{
    pr_info("WORKQUEUE: Called with scancode = 0x%02X, shift = %d\n", last_scancode, shift_pressed);

    switch (last_scancode)
    {
        case 0x08: // '7' key → SHIFT + 7 = '&' → Bitwise AND
            if (shift_pressed) {
                res = c.a & c.b;
                pr_info("Deferred AND (&): %d & %d = %d\n", c.a, c.b, res);
                c.flag = 1;
            }
            break;

        case 0x2B: // '\' key → Bitwise OR
            res = c.a | c.b;
            pr_info("Deferred OR (|): %d | %d = %d\n", c.a, c.b, res);
            c.flag = 1;
            break;

        case 0x07: // '6' key → SHIFT + 6 = '^' → Bitwise XOR
            if (shift_pressed) {
                res = c.a ^ c.b;
                pr_info("Deferred XOR (^): %d ^ %d = %d\n", c.a, c.b, res);
                c.flag = 1;
            }
            break;

        case 0x29: // '`' key → SHIFT + ` = '~' → Bitwise Complement
            res = ~c.a;
            pr_info("Deferred Complement (~): ~%d = %d\n", c.a, res);
            c.flag = 1;
            break;

        default:
            pr_info("Unknown scancode: 0x%02X\n", last_scancode);
            break;
    }
}
static irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(KBD_DATA_PORT);

    pr_info("IRQ: Received scancode = 0x%02X\n", scancode);

    // SHIFT press
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return IRQ_HANDLED;
    }
    // SHIFT release
    else if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = 0;
        return IRQ_HANDLED;
    }

    // Only handle make codes (< 0x80)
    if (scancode < 0x80) {
        last_scancode = scancode;
        pr_info("Scheduling work for scancode = 0x%02X\n", scancode);
        schedule_work(&mywork);
    }

    return IRQ_HANDLED;
}
// File operations
static int my_open(struct inode *inode, struct file *file)
{
    pr_info("Device opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    pr_info("Device closed\n");
    return 0;
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    pr_info("IOCTL: cmd = %u\n", cmd);

    switch (cmd)
    {
        case IOCTL_SET_VALUE:
            if (copy_from_user(&c, (void __user *)arg, sizeof(c)))
                return -EFAULT;
            c.flag = 0;
            pr_info("IOCTL: Received a = %d, b = %d\n", c.a, c.b);
            break;

        case IOCTL_GET_VALUE:
            if (copy_to_user((void __user *)arg, &res, sizeof(int)))
                return -EFAULT;
            pr_info("IOCTL: Sent result = %d\n", res);
            break;

        default:
            pr_info("Invalid IOCTL command: %d\n", cmd);
            return -EINVAL;
    }
    return 0;
}

// File operations structure
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .unlocked_ioctl = my_ioctl,
};

// Module initialization
static int __init keyboard_irq_init(void)
{
    INIT_WORK(&mywork, function);

    int major = register_chrdev(MAJOR_NUM, DEVICE, &fops);
    if (major < 0) {
        pr_err("Failed to register char device\n");
        return major;
    }
    pr_info("Char device registered with major number %d\n", MAJOR_NUM);

    if (request_irq(KEYBOARD_IRQ, keyboard_irq_handler, IRQF_SHARED, "keyboard_irq_handler", (void *)(keyboard_irq_handler))) {
        pr_err("Failed to request IRQ %d\n", KEYBOARD_IRQ);
        unregister_chrdev(MAJOR_NUM, DEVICE);
        return -1;
    }

    pr_info("Keyboard IRQ handler registered on IRQ %d\n", KEYBOARD_IRQ);
    return 0;
}

// Module exit
static void __exit keyboard_irq_exit(void)
{
    unregister_chrdev(MAJOR_NUM, DEVICE);
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_irq_handler));
    flush_scheduled_work();
    pr_info("Module unloaded\n");
}

module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);

