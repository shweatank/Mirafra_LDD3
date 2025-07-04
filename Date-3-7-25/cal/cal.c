#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<asm/io.h>

#define KEYBOARD_IRQ 1 //IRQ number for keyboard
#define KBD_DATA_PORT 0x60 //Port to read keyboard scancode

int a = 20, b = 10;
int shift_pressed = 0;

static irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(KBD_DATA_PORT);

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
            if (shift_pressed)
                printk(KERN_INFO "+ : %d + %d = %d\n", a, b, a + b);
            break;

        case 0x0C: 
            printk(KERN_INFO "- : %d - %d = %d\n", a, b, a - b);
            break;

        case 0x09: 
            if (shift_pressed)
                printk(KERN_INFO "* : %d * %d = %d\n", a, b, a * b);
            break;

        case 0x35: 
                printk(KERN_INFO "/ : %d / %d = %d\n", a, b, a / b);
            break;

        default:
            break;
    }

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
MODULE_AUTHOR("GH");
MODULE_DESCRIPTION("Keyboard IRQ Handler for x86 Linux");
