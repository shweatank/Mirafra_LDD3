#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<linux/timer.h>
#include<linux/smp.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adinarayana");
MODULE_DESCRIPTION("Updated Tasklet Example for x86 linux kernel 5.10+");
//forward decalaration of tasklet function
static void my_tasklet_func(struct tasklet_struct *tasklet);
//declare the tasklet structure
static struct tasklet_struct my_tasklet;
//timer to software interrrupt
static struct timer_list my_timer;
//tasklet function(Bottom Half)
static void my_tasklet_func(struct tasklet_struct *tasklet)
{
	pr_info("TAsklet: Executing in softirq context on CPU %u\n",smp_processor_id());
}
//simulated "interrupt" using timer
static void my_timer_handler(struct timer_list *t)
{
		pr_info("Timer : firing ,simulating interupt -> scheduling tasklet\n");
		//schedule the tasklet (dereferd execution)
		tasklet_schedule(&my_tasklet);
		//restart  the timer for  simulation
		mod_timer(&my_timer,jiffies+msecs_to_jiffies(2000));
}
//module init
static int __init tasklet_init_module(void)
{
	pr_info("tasklet module loaded\n");
	//initalize the tasklet
	tasklet_setup(&my_tasklet,my_tasklet_func);
	//initalize the timer
	timer_setup(&my_timer,my_timer_handler,0);
	mod_timer(&my_timer,jiffies+msecs_to_jiffies(2000));//for 1 sec delay
	return 0;
}
//module exit
static void __exit tasklet_cleanup_module(void)
{
	pr_info("TAsklet Module unloading\n");
	//kill timer and tasklet
	del_timer_sync(&my_timer);
	tasklet_kill(&my_tasklet);
}
module_init(tasklet_init_module);
module_exit(tasklet_cleanup_module);

