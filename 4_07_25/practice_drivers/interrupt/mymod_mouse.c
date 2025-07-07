#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/ioctl.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<asm/io.h>

#define MOUSE_IRQ 12
#define MOUSE_PORT 0x60

static irqreturn_t mouse_irq_handler(int irq,void *dev_id)
{
	unsigned char scancode;
	scancode=inb(MOUSE_PORT);
	printk("mouse_irq_handler: 0x%02X\n",scancode);
	return IRQ_HANDLED;
}

static int __init mouse_init(void)
{
	printk("Loading mouse handler...\n");
	int ret=request_irq(MOUSE_IRQ,mouse_irq_handler,IRQF_SHARED,"mouse_irq_handler",(void *)mouse_irq_handler);
	if(ret)
	{
		printk("mouse_irq_handler failed\n");
		return ret;
	}
	printk("mouse_irq_handler: handler loaded successfully\n");
	return 0;
}

static void __exit mouse_exit(void)
{
	free_irq(MOUSE_IRQ,(void *)mouse_irq_handler);
	printk("mouse_irq_handler: handler removed successfully\n");
}

module_init(mouse_init);
module_exit(mouse_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("a simple interrupt mouse module\n");
