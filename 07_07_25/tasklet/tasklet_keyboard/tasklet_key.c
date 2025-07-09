#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/interrupt.h>
#include<linux/io.h>

#define KEYBOARD_IRQ  1
#define KBD_DATA_PORT 0x60

static struct tasklet_struct my_tasklet;

static irqreturn_t keyboard_irq_handler(int irq, void *dev_id);

static int count=0;

static void my_task_fun(struct tasklet_struct *t)
{
	pr_info("Taskelt executed after 5 keypreses.Total count=%d\n",count);
}
//interrupt handler
irqreturn_t keyboard_irq_handler(int irq,void *dev_id)
{
	char scancode=inb(KBD_DATA_PORT);
	if(scancode<0x08)
		return IRQ_HANDLED;
      
         count++;
	        if(count%5==0)
         	{
		  tasklet_schedule(&my_tasklet);
	        }
	return IRQ_HANDLED;
}
static int __init task_init(void)
{
	pr_info("KERNAL MODULE INTIALIZIED\n");
	 int result;
       //tasklet_setup() should be done before IRQ is registered.
       tasklet_setup(&my_tasklet,my_task_fun);
       result=request_irq(KEYBOARD_IRQ,keyboard_irq_handler,IRQF_SHARED,"keyboard_irq_handler",(void*)&(keyboard_irq_handler));

        if(result)
        {
                printk(KERN_ERR "keyboard_irq:cannot registr irq %d\n",KEYBOARD_IRQ);
                return result;
        }
        printk(KERN_INFO "KEYBOARD_IRQ:IRQ HANDLER REGISTERES SUCCESSFULLY\n");
	  return 0;

}
static void __exit task_exit(void)
{
	pr_info("cleaning up the module--\n");
	tasklet_kill(&my_tasklet);
	free_irq(KEYBOARD_IRQ,(void*)(keyboard_irq_handler));

}
module_init(task_init);
module_exit(task_exit);
MODULE_LICENSE("GPL");


