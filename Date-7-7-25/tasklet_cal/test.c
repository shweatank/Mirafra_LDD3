#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<linux/timer.h>
#include<linux/smp.h>
#include<asm/io.h>

#define KEYBOARD_IRQ 1 //IRQ number for keyboard
#define KBD_DATA_PORT 0x60 //Port to read keyboard scancode
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Techdhaba");
MODULE_DESCRIPTION("Updated Tasklet xample for x86 Linux Kernel 5.10+");

int a = 20, b = 10;
int shift_pressed = 0;
static unsigned char scancode=0;
//Forward declaration of tasklet function
static void my_tasklet_func(struct tasklet_struct *tasklet);

//Declare tasklet_struct
static struct tasklet_struct my_tasklet;

static irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    scancode = inb(KBD_DATA_PORT);
    tasklet_schedule(&my_tasklet);

    return IRQ_HANDLED;
}
//Tasklet function (bottom half)
static void my_tasklet_func(struct tasklet_struct *tasklet)
{
	pr_info("Tasklet: Executing in softirq context on CPU %u\n",smp_processor_id());
	if (scancode == 0x2A || scancode == 0x36)
        {
        shift_pressed = 1;
        return;
        }
        else if (scancode == 0xAA || scancode == 0xB6)
        {
        shift_pressed = 0;
        return;
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

}

//Module init
static int __init tasklet_init_module(void)
{
	pr_info("Tasklet Module Loaded\n");

	//Initialize tasklet
	tasklet_setup(&my_tasklet,my_tasklet_func);
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

//Module exit
static void __exit tasklet_cleanup_module(void)
{
	pr_info("Tasklet Module Unloading\n");
      	free_irq(KEYBOARD_IRQ,(void *)(keyboard_irq_handler));
	printk(KERN_INFO "keyboard_irq: IRQ handler removed\n");
	tasklet_kill(&my_tasklet);
}
module_init(tasklet_init_module);
module_exit(tasklet_cleanup_module);
