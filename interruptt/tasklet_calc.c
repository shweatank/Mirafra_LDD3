#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<asm/io.h>
#include<linux/smp.h>

#define DEVICE_NAME "kbdlogger"
#define KBD_IRQ 1
#define KBD_DATA_PORT 0x60

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pavan");
MODULE_DESCRIPTION("Updated tasklet example for 0x86 Linux kernel 5.10+");

static const char scancode_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`',
    0, '\\','z','x','c','v','b','n','m',',','.','/', 0, '*',
    0, ' '
};
int a=10,b=20,c=0;
char ch;

static void my_tasklet_func(struct tasklet_struct *tasklet);

static struct tasklet_struct my_tasklet;
static irqreturn_t kbd_irq_handler(int irq, void *dev_id);
//timer to simulate interrupt
//static struct timer_list my_timer;

static void my_tasklet_func(struct tasklet_struct *tasklet)
{
        pr_info("Tasklet: executing in softirq context on cpu %u\n",smp_processor_id());
	switch(ch)
	{
		case 'a':
			pr_info("c= %c\n",ch);
			c=a+b;
			pr_info("Tasklet: result=%d\n",c);
			break;
		 case 's':
                        pr_info("c= %c\n",ch);
                        c=a-b;
                        pr_info("Tasklet: result=%d\n",c);
			break;
		case 'm':
			pr_info("c= %c\n",ch);
                        c=a*b;
                        pr_info("Tasklet: result=%d\n",c);
                        break;
		case 'd':
			pr_info("c= %c\n",ch);
                        c=a/b;
                        pr_info("Tasklet: result=%d\n",c);
                        break;
		default :
			pr_info("Tasklet: invalid key: %c\n",ch);
			break;
	}
}

//simultaed "interrupt" using a timer
/*static void my_timer_handler(struct timer_list *t)
{
        pr_info("Timer: firing,simulating interrupt -> scheduling tasklet\n");


        tasklet_schedule(&my_tasklet);
        mod_timer(&my_timer,jiffies + msecs_to_jiffies(2000));
}*/

static irqreturn_t kbd_irq_handler(int irq, void *dev_id)
{
    unsigned char sc = inb(KBD_DATA_PORT);
    if (sc < 128) {
        ch =scancode_ascii[sc];
	}
    tasklet_schedule(&my_tasklet);
    return IRQ_HANDLED;

}


static int __init tasklet_init_module(void)
{
        pr_info("Tasklet Module loaded\n");
	int ret;
	ret = request_irq(KBD_IRQ, kbd_irq_handler, IRQF_SHARED, DEVICE_NAME, (void *)(kbd_irq_handler));
    if (ret) {
        pr_err("Failed to register IRQ\n");
        return ret;
    }
        //initilizing tasklet
        tasklet_setup(&my_tasklet,my_tasklet_func);

        //initilizing timer
        //timer_setup(&my_timer,my_timer_handler,0);
        //mod_timer(&my_timer,jiffies + msecs_to_jiffies(2000));

        return 0;
}

static void __exit tasklet_cleanup_module(void)
{
        pr_info("tasklet module unloading\n");

	free_irq(KBD_IRQ, (void *)(kbd_irq_handler));
        //del_timer_sync(&my_timer);
        tasklet_kill(&my_tasklet);
}

module_init(tasklet_init_module);
module_exit(tasklet_cleanup_module);

