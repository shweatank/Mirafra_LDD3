#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<linux/timer.h>
#include<linux/smp.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BHARATH");
MODULE_DESCRIPTION("Updated Tasklet for x86 linux kernel 5.10+");

static void my_tasklet_func(struct tasklet_struct *tasklet);

static struct tasklet_struct my_tasklet;

static struct timer_list my_timer;

static void my_tasklet_func(struct tasklet_struct *tasklet)
{
	pr_info("Tasklet: Executing in softirq context on CPU %u\n",smp_processor_id());

}

static void my_timer_handler(struct timer_list *t)
{
	pr_info("Timer: Firing, simulating interrupt -> scheduling tasklet\n");

tasklet_schedule(&my_tasklet);

mod_timer(&my_timer,jiffies + msecs_to_jiffies(2000));
}
//Module init
static int __init tasklet_init_module(void)
{
	pr_info("Tasklet Module Loaded\n");
          //Initialise tasklet
	tasklet_setup(&my_tasklet,my_tasklet_func);
        // Initialise timer
	timer_setup(&my_timer,my_timer_handler,0);
	mod_timer(&my_timer,jiffies + msecs_to_jiffies(2000));

	return 0;
}
//Module exit

static void __exit tasklet_cleanup_module(void)
{
	pr_info("Tasklet Module Unloading\n");
	//kill timer and tasklet
	del_timer_sync(&my_timer);
	tasklet_kill(&my_tasklet);
}

module_init(tasklet_init_module);
module_exit(tasklet_cleanup_module);

