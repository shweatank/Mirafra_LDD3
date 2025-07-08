#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<linux/timer.h>
#include<linux/smp.h>
#include<asm/io.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("BHARATH");
MODULE_DESCRIPTION("Updated Tasklet for x86 linux kernel 5.10+");
#define KEY 0x60
static void my_tasklet_func(struct tasklet_struct *tasklet);
static int shift_pressed=0;
char scancode_map[128]={
	0,27,'1','2','3','4','5','6','7','8','9','0','-','+','\b','\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a','x','c','v','b','n','m',',','.','/',0,'*',0,' ',0
};
static struct tasklet_struct my_tasklet;
static unsigned int scancode;

static void my_tasklet_func(struct tasklet_struct *tasklet)
{
	pr_info("Tasklet: Executing in softirq context on CPU %u\n",smp_processor_id());
	 int a = 12, b = 5;
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



static irqreturn_t keyboard_irq_handler(int irq,void *dev_id)
{
	scancode = inb(KEY);
                  tasklet_schedule(&my_tasklet);
	return IRQ_HANDLED;
}
//Module init
static int __init tasklet_init_module(void)
{
	pr_info("Tasklet Module Loaded\n");
          //Initialise tasklet
	tasklet_setup(&my_tasklet,my_tasklet_func);
        
	if(request_irq(1,keyboard_irq_handler,IRQF_SHARED,"keyboard_irq_handler",(void *)(keyboard_irq_handler))){
		pr_err("Failed to register IRQ\n");
		return -EIO;
	}

	return 0;
}
//Module exit

static void __exit tasklet_cleanup_module(void)
{
	pr_info("Tasklet Module Unloading\n");
	//kill tasklet
	free_irq(1,(void *)(keyboard_irq_handler));
	tasklet_kill(&my_tasklet);
}

module_init(tasklet_init_module);
module_exit(tasklet_cleanup_module);

