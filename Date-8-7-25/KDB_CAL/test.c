#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/interrupt.h>
#include<linux/init.h>
#include<linux/delay.h>
#include<asm/io.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("HARSHA");

#define KBD_DATA_PORT 0x60 
#define KBD_IRQ 1
int a = 20, b = 10;
int shift_pressed = 0;
static unsigned char scancode=0;
static int dev_id=1;

static irqreturn_t kb_top_handler(int irq,void *dev_id)
{
	pr_info("kbd_threaded:[TOP HALF] IRQ %d received\n",irq);
	scancode = inb(KBD_DATA_PORT);
	return IRQ_WAKE_THREAD;
}

static irqreturn_t kb_thread_handler(int irq,void *dev_id)
{
	pr_info("Tasklet: Executing in softirq context on CPU %u\n",smp_processor_id());
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

static int __init kbd_irq_init(void)
{
	int ret;
	pr_info("kbd_threaded is loading\n");

	ret=request_threaded_irq(KBD_IRQ,kb_top_handler,kb_thread_handler,IRQF_SHARED,"kbd_threaded_irq",(void *)&dev_id);
	if(ret)
	{
		pr_err("kbd_threaded:failed to register IRQ %d\n",KBD_IRQ);
		return ret;
	}
	pr_info("kbd_threaded IRQ %d registered succesfully\n",KBD_IRQ);
	return 0;
}
static void __exit kbd_irq_exit(void)
{
	free_irq(KBD_IRQ,(void *)&dev_id);
	pr_info("module is unloaded\n");
}
module_init(kbd_irq_init);
module_exit(kbd_irq_exit);


