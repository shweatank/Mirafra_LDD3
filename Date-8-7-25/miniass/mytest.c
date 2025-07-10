#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <asm/io.h>	
MODULE_LICENSE("GPL");
MODULE_AUTHOR("HARSHA");
MODULE_DESCRIPTION("Task with kthread + waitqueue + Keywordinterrupt");
#define KBD_DATA_PORT 0x60 
#define DEVICE_NAME "mywait"
#define KBD_IRQ 1

static int shift_pressed = 0;
static unsigned char scancode=0;
static int dev_id=1;
static dev_t dev;
static struct cdev my_cdev;
static DECLARE_WAIT_QUEUE_HEAD(wq);
static int flag = 0;  
static struct class *my_class;
static char result[128];
static int a=0,b=0,res=0;

static irqreturn_t kb_top_handler(int irq,void *dev_id)
{
	pr_info("kbd_threaded:[TOP HALF] IRQ %d received\n",irq);
	scancode = inb(KBD_DATA_PORT);
	return IRQ_WAKE_THREAD;
}

static irqreturn_t kb_thread_handler(int irq,void *dev_id)
{
	pr_info("The scancode :%d\n",scancode);
	if (scancode == 0x2A || scancode == 0x36)
        {
        shift_pressed = 1;
        return IRQ_HANDLED;
        }
        else if (scancode == 0xAA || scancode == 0xB6)
        {
        shift_pressed = 0;
        return IRQ_HANDLED;
        }
    switch (scancode)
    {
    case 0x0D: 
        if (shift_pressed) {
            res = a + b;
            pr_info("Addition performed: %d + %d = %d\n", a, b, res);
            flag = 1;
        }
        break;
    case 0x0C: 
        res = a - b;
        pr_info("Subtraction performed: %d - %d = %d\n", a, b, res);
        flag = 1;
        break;
    case 0x09: 
        if (shift_pressed) 
	{
            res = a * b;
            pr_info("Multiplication performed: %d * %d = %d\n", a, b, res);
            flag = 1;
        }
        break;
    case 0x35: 
	if (b != 0) 
	{
        res = a / b;
        pr_info("Division performed: %d / %d = %d\n", a, b, res);
	} 
	else 
	{
        res = 0;
        pr_info("Division by zero attempted: %d / %d — returning 0\n", a, b);
    	}
    	flag = 1;
    	break;
    default:
        pr_info("Unmapped scancode received: 0x%02X\n", scancode);
        return IRQ_HANDLED;
    }

    if (flag) 
    {
        wake_up_interruptible(&wq);
    }
    return IRQ_HANDLED;
}
static ssize_t my_read(struct file *filp, char __user *buf,size_t len, loff_t *off)
{
    printk(KERN_INFO "mywait: Reader waiting...\n");
    // Wait until flag is set
    wait_event_interruptible(wq, flag != 0);
    if (*off > 0)
        return 0; 
     snprintf(result, sizeof(result), "Result: %d\n",res);
     if (copy_to_user(buf, result, sizeof(result)))
        return -EFAULT;
    flag = 0;
    return sizeof(result);
}
static ssize_t my_write(struct file *filp, const char __user *buf,size_t len, loff_t *off)
{
	char readbuf[128];
	if (len >= sizeof(readbuf))
        return -EINVAL;
        if (copy_from_user(readbuf, buf, sizeof(readbuf)))
        return -EFAULT;
	readbuf[len] = '\0';
	pr_info("The Data is recevied\n");
	sscanf(readbuf, "%d %d", &a, &b);
	pr_info("The Values are a=%d,b=%d\n",a,b);	
        printk(KERN_INFO "mywait: Writer waking up reader\n");
        return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
};

static int __init kbd_irq_init(void)
{
	int ret;
	pr_info("kbd_threaded is loading\n");
       	alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
       	cdev_init(&my_cdev, &fops);
       	cdev_add(&my_cdev, dev, 1);
	my_class = class_create(DEVICE_NAME);
	device_create(my_class, NULL, dev, NULL, DEVICE_NAME);
	ret=request_threaded_irq(KBD_IRQ,kb_top_handler,kb_thread_handler,IRQF_SHARED,"kbd_threaded_irq",(void *)&dev_id);
	if(ret)
	{
		pr_err("kbd_threaded:failed to register IRQ %d\n",KBD_IRQ);
		return ret;
	}
	pr_info("kbd_threaded IRQ %d registered succesfully\n",KBD_IRQ);
	return 0;
}
static void __exit kbd_irq_exit(void)
{
	free_irq(KBD_IRQ,(void *)&dev_id);
       	device_destroy(my_class, dev);
	class_destroy(my_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("module is unloaded\n");
}
module_init(kbd_irq_init);
module_exit(kbd_irq_exit);
