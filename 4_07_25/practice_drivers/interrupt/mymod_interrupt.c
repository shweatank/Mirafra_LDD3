#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<asm/io.h>

#define KEYBOARD_IRQ 1
#define KBD_PORT 0x60

irqreturn_t keyboard_irq_handler(int irq,void *dev_id)
{
	unsigned char scancode;
	scancode=inb(KBD_PORT);
	printk("Keyboard irq scancode: 0x%02X\n",scancode);
	return IRQ_HANDLED;
}

static int __init interrupt_init(void)
{
	printk("Loading custom keyboard irq handler\n");
	int result=request_irq(KEYBOARD_IRQ,keyboard_irq_handler,IRQF_SHARED,"keyboard_irq_handler",(void *)keyboard_irq_handler);
	if(result)
	{
		printk("keyboard irq:%d cannot register irq\n",KEYBOARD_IRQ);
		return result;
	}
	printk("Keyboard irq handler registered successfully\n");
	return 0;
}

static void __exit interrupt_exit(void)
{
	free_irq(KEYBOARD_IRQ,(void *)keyboard_irq_handler);
	printk("keyboard irq handler removed successfully\n");
}

module_init(interrupt_init);
module_exit(interrupt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("a simple interrupt module");
