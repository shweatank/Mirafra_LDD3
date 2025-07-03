#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/interrupt.h>
#include<asm/io.h>

#define KEYBOARD_IRQ 1        //IRQ NUMBER KEYBOARD

//Port 0x60 is the standard data register for PS/2 keyboard input.
#define KBD_DATA_PORT 0x60    //PORT TO READ KEYBOARD SCANNED

static unsigned long last_jiffy = 0;


//interrupt handler
irqreturn_t keyboard_irq_handler(int irq,void *dev_id)
{
	unsigned char scancode;
	int a=10,b=23,result=0;

	//inb() reads a byte from the I/O port — used here to read the raw scancode from the keyboard controller.
	scancode =inb(KBD_DATA_PORT);

	// Ignore break codes (key release)
    if (scancode & 0x80)
        return IRQ_HANDLED;

    // Debounce: ignore repeated presses too fast
    /*if (time_before(jiffies, last_jiffy + DEBOUNCE_JIFFIES))//Checks if event is too soon (bounce)
        return IRQ_HANDLED;

    last_jiffy = jiffies;//Updates last valid press time*/
    switch(scancode)
    {
	    case 0x0D:result=a+b;
		     break;
	    case 0X0C:result=a-b;
		     break;
	    case 0X09:result=a*b;
	             break;
	    default:printk(KERN_ERR"INVALID SCANCODE\n");
		    break;
    }
	printk(KERN_INFO "KEYBOARD IRQ:scancode =0x%02x and result:%d\n",scancode,result);
	return IRQ_HANDLED;
}
static int __init keyboard_irq_init(void)
{
	int result;
	printk(KERN_INFO "LOADING CUSTOM KEYBOARD_IRQ HANDLER---\n");

        // request irq keyboard interrupt
	result=request_irq(KEYBOARD_IRQ,keyboard_irq_handler,IRQF_SHARED,"keyboard_irq_handler",(void*)(keyboard_irq_handler));
	 // IRQ number (1 = keyboard)
        // handler function
         // shared (keyboard already used)
         // name shown in /proc/interrupts
          // device ID for shared IRQ

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



