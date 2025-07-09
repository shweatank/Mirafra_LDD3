#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/interrupt.h>
#include<linux/timer.h>
#include<linux/smp.h>
#include<asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SOWMYA");
MODULE_DESCRIPTION("UPDATED TASKLET EXAMPLE FOR X86 LINUX KERNEL");

#define KEYBOARD_IRQ   1

#define KBD_DATA_PORT 0X60

//forward declaaration of tasklet function
static void my_tasklet_func(struct tasklet_struct *tasklet);

//declare tasklet_struct
static struct tasklet_struct my_tasklet;


//tasklet function bottonhalf
static void my_tasklet_func(struct tasklet_struct *tasklet)
{
	pr_info("tasklet:executing in softirq on cpu %u\n",smp_processor_id());
	char scancode = inb(KBD_DATA_PORT);
        unsigned char keycode = scancode & 0x7F;  // mask release bit
        bool is_release = scancode & 0x80;

        if (keycode > 0x58) {
              pr_info("INVALID SCANCODE: 0x%x\n", scancode);
	}

	int a=20,b=13,result=0;

	switch(keycode)
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
        printk(KERN_INFO "KEYBOARD IRQ:result:%d\n",result);

}

//interrupt handler
irqreturn_t keyboard_irq_handler(int irq,void *dev_id)
{
        pr_info("keyboard interrupt has been triggered and setting it to bottom half\n");

	
 	//schedule the tasklet (deferred executin)
        tasklet_schedule(&my_tasklet);

	return IRQ_HANDLED;

}

//module_init
static int __init tasklet_init_module(void)
{
	int result;
        pr_info("tasklet module loaded\n");

	// request irq keyboard interrupt
        result=request_irq(KEYBOARD_IRQ,keyboard_irq_handler,IRQF_SHARED,"keyboard_irq_handler",(void*)(keyboard_irq_handler));
      
        if(result)
        {
                printk(KERN_ERR "keyboard_irq:cannot registr irq %d\n",KEYBOARD_IRQ);
                return result;
        }
        printk(KERN_INFO "KEYBOARD_IRQ:IRQ HANDLER REGISTERES SUCCESSFULLY\n");


        //initialize the tasklet
        tasklet_setup(&my_tasklet,my_tasklet_func);

        return 0;
}
static void __exit tasklet_cleanup_module(void)
{
        pr_info("tasklet module unloading\n");

        //kill  tasklet
        tasklet_kill(&my_tasklet);

	  free_irq(KEYBOARD_IRQ,(void*)(keyboard_irq_handler));
        printk(KERN_INFO "KEYBOARD_IRQ:IRQ HANDLER REMOVED\n");

}
module_init(tasklet_init_module);
module_exit(tasklet_cleanup_module);


