#include<linux/kernel.h>
#include<linux/interrupt.h>
#include<linux/module.h>
static struct tasklet_struct my_tasklet;

static void tasklet_fun(struct tasklet_struct *t)
{
	printk("I am in bottom half\n");
}

static int __init mymod_init(void)
{
	printk("Module loaded successfully\n");
	tasklet_setup(&my_tasklet,tasklet_fun);
	tasklet_schedule(&my_tasklet);
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
MODULE_DESCRIPTION("a simple tasklet module");

