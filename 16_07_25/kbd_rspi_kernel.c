#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/interrupt.h>
#include<asm/io.h>

#define DEVICE_NAME "task"

#define KEYBOARD_IRQ 1        //IRQ NUMBER KEYBOARD

//Port 0x60 is the standard data register for PS/2 keyboard input.
#define KBD_DATA_PORT 0x60    //PORT TO READ KEYBOARD SCANNED

static unsigned char scancode;
static int major=0;
static int dev_open(struct inode *inode,struct file *file)
{
        printk(KERN_INFO "simplle_char_dev:device opened\n");
        return 0;
}
static int dev_release(struct inode *inode,struct file *file)
{
         printk(KERN_INFO "simplle_char_dev:device closed\n");
        return 0;
}
//interrupt handler
irqreturn_t keyboard_irq_handler(int irq,void *dev_id)
{
	//inb() reads a byte from the I/O port — used here to read the raw scancode from the keyboard controller.
	scancode =inb(KBD_DATA_PORT);

	// Ignore break codes (key release)
    if (scancode & 0x80)
        return IRQ_HANDLED;

    return IRQ_HANDLED;
}

ssize_t dev_read(struct file *filep, char __user *user_buffer, size_t count, loff_t *offset) {
    if (*offset >= 1)
        return 0;  // EOF

    if (copy_to_user(user_buffer, &scancode, 1))
        return -EFAULT;

    *offset += 1;
    return 1;
}
struct file_operations fops ={
        .open=dev_open,
        .release=dev_release,
        .read=dev_read,
        //.write=dev_write,
};
static int __init keyboard_irq_init(void)
{
	int result;
	printk(KERN_INFO "LOADING CUSTOM KEYBOARD_IRQ HANDLER---\n");

	major=register_chrdev(0,DEVICE_NAME,&fops);
        if(major<0)
        {
         printk(KERN_ALERT "simplle_char_dev:fail to register\n");
        return major;
        }
        printk(KERN_INFO"simple_char_dev:registered with major number %d\n",major);

        // request irq keyboard interrupt
	result=request_irq(KEYBOARD_IRQ,keyboard_irq_handler,IRQF_SHARED,"keyboard_irq_handler",(void*)(keyboard_irq_handler));

	if(result)
	{
		printk(KERN_ERR "keyboard_irq:cannot registr irq %d\n",KEYBOARD_IRQ);
		return result;
	}
	printk(KERN_INFO "KEYBOARD_IRQ:IRQ HANDLER REGISTERES SUCCESSFULLY\n");
	return 0;
}
static void __exit keyboard_irq_exit(void)
{
	free_irq(KEYBOARD_IRQ,(void*)(keyboard_irq_handler));
	printk(KERN_INFO "KEYBOARD_IRQ:IRQ HANDLER REMOVED\n");
}
module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);
MODULE_LICENSE("GPL");

