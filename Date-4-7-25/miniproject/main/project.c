// Kernel Module: ioctl_keyboard_filter.c (Updated)
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <asm/io.h>

#define DEVICE "ioctl_demo"
#define MAJOR_NUM 100
#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60
#define BUF_SIZE 256

#define IOCTL_SEND_STRING _IOW(MAJOR_NUM, 2, char *)
#define IOCTL_GET_STRING _IOR(MAJOR_NUM, 3, char *)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Harsha");
MODULE_DESCRIPTION("Keyboard IRQ Filter with IOCTL and Char Device Driver");

static char input_buf[BUF_SIZE];
static char a[BUF_SIZE]; 
static char b[BUF_SIZE]; 
static char c[BUF_SIZE]; 
static char selected_key = 'z';
static int ready_flag = 0;

// IRQ handler
static irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(KBD_DATA_PORT);
    pr_info("IRQ: Scancode = 0x%02X\n", scancode);

    if (scancode < 0x80)
    {
        switch (scancode) 
	{
            case 0x02: selected_key = 'a'; ready_flag = 1;
                       pr_info("Selected group A (alphabets)\n"); break; 
            case 0x03: selected_key = 'b'; ready_flag = 1;
                       pr_info("Selected group B (digits)\n"); break;    
            case 0x04: selected_key = 'c'; ready_flag = 1;
                       pr_info("Selected group C (special chars)\n"); break; 
            default: break;
        }
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
    switch (cmd)
    {
        case IOCTL_SEND_STRING:
            if (copy_from_user(input_buf, (char __user *)arg, BUF_SIZE))
                return -EFAULT;

            memset(a, 0, BUF_SIZE);
            memset(b, 0, BUF_SIZE);
            memset(c, 0, BUF_SIZE);
	    selected_key = '0';
            ready_flag = 0;

	    int i, j = 0, k = 0, l = 0;
            for (i = 0; input_buf[i] != '\0' && i < BUF_SIZE; ++i) 
	    {
		    char ch = input_buf[i];
                    if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
                        a[j++] = ch;
                    else if (ch >= '0' && ch <= '9')
                        b[k++] = ch;
                    else
                        c[l++] = ch;
            }

            pr_info("Received string: %s\n", input_buf);
            pr_info("A: %s\n", a);
            pr_info("B: %s\n", b);
            pr_info("C: %s\n", c);
            break;

        case IOCTL_GET_STRING:
            if (!ready_flag)
                return -EAGAIN;
            else {
                char *result = NULL;
                switch (selected_key)
		{
                    case 'a': result = a; break;
                    case 'b': result = b; break;
                    case 'c': result = c; break;
                    default: return -EINVAL;
                }

                if (copy_to_user((char __user *)arg, result, BUF_SIZE))
                    return -EFAULT;

                pr_info("Sent group '%c': %s\n", selected_key, result);
                ready_flag = 0;
            }
            break;

        default:
            pr_info("Invalid IOCTL command: %u\n", cmd);
            return -EINVAL;
    }

    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .unlocked_ioctl = my_ioctl,
};

static int __init keyboard_irq_init(void)
{
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

static void __exit keyboard_irq_exit(void)
{
    unregister_chrdev(MAJOR_NUM, DEVICE);
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_irq_handler));
    pr_info("Module unloaded\n");
}

module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);

