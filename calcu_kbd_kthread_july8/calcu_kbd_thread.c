#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/interrupt.h>
#include<linux/init.h>
#include<linux/delay.h>
#include<asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Techdhaba");
MODULE_DESCRIPTION("Keyboard IRQ with threaded handler");
MODULE_VERSION("1.0");

unsigned char scancode;
#define KBD_IRQ 1
#define KBD_DATA_PORT 0x60 //Port to read keyboard scancode
static int dev_id = 1;

// ---Top-half : quick acknowledge ---
static irqreturn_t kb_top_handler(int irq,void *dev_id)
{
	//unsigned char scancode;

	//Read scancode from post 0x60
	scancode=inb(KBD_DATA_PORT);

	pr_info("kbd_threaded: [TOP HALF] IRQ %d received\n",irq);
	return IRQ_WAKE_THREAD; //Schedule bottom half
}

//--- Bottom-Half: threaded handler ---
static irqreturn_t kb_threaded_handler(int irq,void *dev_id)
{
	pr_info("kbd_threaded: [BOTTOM HALF] Handling key press...\n");

	//msleep(500); //Simulated long processing
	switch(scancode)
	{
		case 0x9E:printk(KERN_INFO "Addition = %d",10+5);	
			  break;
		case 0x9F:printk(KERN_INFO"Substraction = %d",10-5);
			  break;
		case 0xB2:printk(KERN_INFO"Multiplication = %d\n",10*5);
			  break;
		case 0xA0:printk(KERN_INFO"Division = %d",10/5);
			  break;
		default :printk(KERN_INFO"Invalid choice");
	}
	printk(KERN_INFO "Keyboard IRQ: Scancode = 0x%02X\n",scancode);
	pr_info("kbd_threaded: [BOTTOM HALF] Done processing\n");

	msleep(500); //Simulated long processing
	return IRQ_HANDLED;
}

static int __init kbd_irq_init(void)
{
	int ret;
	pr_info("kbd_threaded: Loading module\n");

	ret = request_threaded_irq(KBD_IRQ,
				   kb_top_handler, //Top Half
				   kb_threaded_handler, // Bottom Half
				   IRQF_SHARED,
				   "kbd_threaded_irq",
				   (void *)&dev_id); // dev_id
	if(ret)
	{
		pr_err("kbd_threaded: Failed to register IRQ %d\n",KBD_IRQ);
		return ret;
	}

	pr_info("kbd_threaded: IRQ %d registered successfully\n",KBD_IRQ);
	return 0;
}
static void __exit kbd_irq_exit(void)
{
	free_irq(KBD_IRQ,(void *)&dev_id);
	pr_info("kbd_threaded: IRQ %d freed , module unloaded\n",KBD_IRQ);
}
module_init(kbd_irq_init);
module_exit(kbd_irq_exit);

