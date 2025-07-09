#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/interrupt.h>

static struct tasklet_struct my_tasklet;

static void my_task(struct tasklet_struct *t)
{
	printk(KERN_INFO "TASKLET EXECUTED WITH THE DATA\n");
}
//DECLARE_TASKLET(my_tasklet,my_task,123);

static int __init task_init(void)
{
	printk(KERN_INFO "scheduling tasklet\n");
	tasklet_setup(&my_tasklet, my_task); // New API
	tasklet_schedule(&my_tasklet);
	return 0;
}
static void __exit task_exit(void)
{
	printk(KERN_INFO "killing the tasklet--\n");
	tasklet_kill(&my_tasklet);
}
module_init(task_init);
module_exit(task_exit);

MODULE_LICENSE("GPL");

