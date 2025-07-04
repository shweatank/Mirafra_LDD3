#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<asm/io.h>

#define KEYBOARD_IRQ 1 //irq no for keyboard
#define KBD_DATA_PORT 0x60 //port to read keyboard scancode

//interrupt handler

irqreturn_t keyboard_irq_handler(int irq,void *dev_id)
{
	static unsigned char last_scancode = 0;
        unsigned char scancode;

        //read scancode from port 0x60
        scancode = inb(KBD_DATA_PORT);
        if (scancode != last_scancode) {
                printk(KERN_INFO "Keyboard IRQ: scancode = 0x%02X\n", scancode);
                last_scancode = scancode;
        }

	return IRQ_HANDLED;
}

static int __init keyboard_irq_init(void)
{
	int result;
	 printk(KERN_INFO "loading custom keyboard IRQ handler...\n");

	 //requesting iRQ 1
	 
	 result=request_irq(KEYBOARD_IRQ,keyboard_irq_handler,IRQF_SHARED,"keyboard_irq_handler",(void *)(keyboard_irq_handler));

	 if(result)
	 {
		 printk(KERN_ERR "keyboard_irq: cannot register IRQ %d\n",KEYBOARD_IRQ);
		 return result;
	 }

	 printk(KERN_INFO "keyboard_irq: IRQ handler registered sucessfully.\n");
	 return 0;
}

static void __exit keyboard_irq_exit(void)
{
	free_irq(KEYBOARD_IRQ,(void *)(keyboard_irq_handler));
	printk(KERN_INFO "keyboard_irq: IRQ handler removed. \n");
}

module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("pavan");
MODULE_DESCRIPTION("keyboard custom device driver");
