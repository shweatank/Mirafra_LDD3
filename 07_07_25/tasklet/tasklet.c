#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/interrupt.h>
#include<linux/timer.h>
#include<linux/smp.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SOWMYA");
MODULE_DESCRIPTION("UPDATED TASKLET EXAMPLE FOR X86 LINUX KERNEL");

//forward declaaration of tasklet function
static void my_tasklet_func(struct tasklet_struct *tasklet);

//declare tasklet_struct
static struct tasklet_struct my_tasklet;

//timer to simulate interrupt
static struct timer_list my_timer;

//tasklet function bottonhalf
static void my_tasklet_func(struct tasklet_struct *tasklet)
{
	pr_info("tasklet:executing in softirq on cpu %u\n",smp_processor_id());
}

//simulated"interrupt" using a timer
static void my_timer_handler(struct timer_list *t)
{
	pr_info("timer:firing,simulating interrupt-->scheduling tasklet\n");

//schedule the tasklet (deferred executin)
tasklet_schedule(&my_tasklet);

//restart the timer for periodic simulation
mod_timer(&my_timer,jiffies + msecs_to_jiffies(2000));
}

//module_init
static int __init tasklet_init_module(void)
{
	pr_info("tasklet module loaded\n");

	//initialize the tasklet
	tasklet_setup(&my_tasklet,my_tasklet_func);

	//initialize the timer
	timer_setup(&my_timer,my_timer_handler,0);
        
	mod_timer(&my_timer,jiffies + msecs_to_jiffies(2000));

	return 0;
}
static void __exit tasklet_cleanup_module(void)
{
	pr_info("tasklet module unloading\n");

	//kill timer and tasklet
	del_timer_sync(&my_timer);
	tasklet_kill(&my_tasklet);
}
module_init(tasklet_init_module);
module_exit(tasklet_cleanup_module);

/*spin_lock_irqsave()	Used to lock critical sections in SMP-safe way
Per-CPU variables	Prevent data sharing between CPUs
CPU Affinity	Bind tasks to specific CPU
smp_processor_id()	Get current CPU ID
smp_call_function()	Run function on other CPUs
CONFIG_SMP	Kernel build flag for SMP support*/
