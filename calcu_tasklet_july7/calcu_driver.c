#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<linux/timer.h>
#include<linux/smp.h>
#include<asm/io.h>

#define KEYBOARD_IRQ 1 //IRQ number for keyboard
#define KBD_DATA_PORT 0x60 //Port to read keyboard scancode
unsigned char scancode;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Techdhaba");
MODULE_DESCRIPTION("Updated Tasklet xample for x86 Linux Kernel 5.10+");

//Forward declaration of tasklet function
static void my_tasklet_func(struct tasklet_struct *tasklet);

//Declare tasklet_struct
static struct tasklet_struct my_tasklet;


//Tasklet function (bottom half)
static void my_tasklet_func(struct tasklet_struct *tasklet)
{
	//pr_info("Tasklet: Executing in softirq context on CPU %u\n",smp_processor_id());
	pr_info("Entered tasklet\n");
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
}

static irqreturn_t keyboard_irq_handler(int irq,void *dev_id)
{
	scancode=inb(KBD_DATA_PORT);
	pr_info("Keyboard Iterrupt triggered\n");
	tasklet_schedule(&my_tasklet);
	return IRQ_HANDLED;
}


//Module init
static int __init tasklet_init_module(void)
{
	pr_info("Tasklet Module Loaded\n");

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

	//Initialize tasklet
	tasklet_setup(&my_tasklet,my_tasklet_func);

	return 0;
}

//Module exit
static void __exit tasklet_cleanup_module(void)
{
	pr_info("Tasklet Module Unloading\n");

	free_irq(KEYBOARD_IRQ,(void *)(keyboard_irq_handler));
	tasklet_kill(&my_tasklet);
}
module_init(tasklet_init_module);
module_exit(tasklet_cleanup_module);
