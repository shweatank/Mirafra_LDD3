#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<asm/io.h>

#define KEYBOARD_IRQ 1 //IRQ number for keyboard
#define KBD_DATA_PORT 0x60 //Port to read keyboard scancode

//Interrupt handler
int i=0;
irqreturn_t keyboard_irq_handler(int irq,void *dev_id)
{

	unsigned char scancode;
	if(i==0)
	scancode=inb(KBD_DATA_PORT);
	i=1;
	for(int j=0;j<1000000;j++);
	printk(KERN_INFO "Keyboard IRQ: Scancode = 0x%02X\n",scancode);
	i=0;
	return IRQ_HANDLED;
}

static int __init keyboard_irq_init(void)
{
	int result;

	printk(KERN_INFO "Loading custom keyboard IRQ handler...\n");

	//Request IRQ 1 (keyboard interrupt)
	result = request_irq(KEYBOARD_IRQ,keyboard_irq_handler,IRQF_SHARED,"keyboard_irq_handler",(void *)(keyboard_irq_handler));
	if(result)
	{
		printk(KERN_ERR "keyboard_irq: Cannot register IRQ %d\n",KEYBOARD_IRQ);
		return result;
	}
	printk(KERN_INFO " keyboard_irq: IRQ handler registered successfully.\n");
	return 0;
}

static void __exit keyboard_irq_exit(void)
{
	free_irq(KEYBOARD_IRQ,(void *)(keyboard_irq_handler));
	printk(KERN_INFO "keyboard_irq: IRQ handler removed\n");
}
module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Keyboard IRQ Handler for x86 Linux");
