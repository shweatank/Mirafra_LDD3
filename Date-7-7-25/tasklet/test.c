#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<linux/timer.h>
#include<linux/smp.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Techdhaba");
MODULE_DESCRIPTION("Updated Tasklet xample for x86 Linux Kernel 5.10+");

//Forward declaration of tasklet function
static void my_tasklet_func(struct tasklet_struct *tasklet);

//Declare tasklet_struct
static struct tasklet_struct my_tasklet;

//Timer to simulate interrupt
static struct timer_list my_timer;

//Tasklet function (bottom half)
static void my_tasklet_func(struct tasklet_struct *tasklet)
{
	pr_info("Tasklet: Executing in softirq context on CPU %u\n",smp_processor_id());
}
//Simulted "interrupt" using a timer
static void my_timer_handler(struct timer_list *t)
{
	pr_info("Timer: Firing, simulating interrupt -> scheduling tasklet\n");

	//Schedule the tasklet (deferred execution)
	tasklet_schedule(&my_tasklet);

	//Restart the timer for periodic simulation
	mod_timer(&my_timer,jiffies + msecs_to_jiffies(2000));
}
//Module init
static int __init tasklet_init_module(void)
{
	pr_info("Tasklet Module Loaded\n");

	//Initialize tasklet
	tasklet_setup(&my_tasklet,my_tasklet_func);

	//Initialize timer
	timer_setup(&my_timer,my_timer_handler,0);
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));

	return 0;
}

//Module exit
static void __exit tasklet_cleanup_module(void)
{
	pr_info("Tasklet Module Unloading\n");

	//Kill timer and tasklet
	del_timer_sync(&my_timer);
	tasklet_kill(&my_tasklet);
}
module_init(tasklet_init_module);
module_exit(tasklet_cleanup_module);
