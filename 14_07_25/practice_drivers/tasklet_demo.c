#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/interrupt.h>
static struct tasklet_struct my_tasklet;

static void tasklet_fun(struct tasklet_struct *d)
{
	printk("I am in tasklet\n");
}
static int __init mymod_init(void)
{
	printk("Tasklet demo\n");
	tasklet_setup(&my_tasklet,tasklet_fun);
	tasklet_schedule(&my_tasklet);
	printk("Module loaded successfully\n");
	return 0;
}

static void __exit mymod_exit(void)
{
	printk("Module removed successfully\n");
	tasklet_kill(&my_tasklet);
}
module_init(mymod_init);
module_exit(mymod_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("a simple module\n");
