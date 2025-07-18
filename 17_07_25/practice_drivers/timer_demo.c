#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/timer.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("a simple module");

static struct timer_list timer;

void my_fun(struct timer_list *t)
{
	pr_info("Timer callback fired at jiffies = %ld\n",jiffies);
}

static int __init mymod_init(void)
{
	pr_info("Initializing the periodic timer module\n");
	timer_setup(&timer,my_fun,0);
	mod_timer(&timer,jiffies+msecs_to_jiffies(2000));
	return 0;
}

static void __exit mymod_exit(void)
{
	pr_info("Exitting the timer module\n");
	del_timer(&timer);
}

module_init(mymod_init);
module_exit(mymod_exit);

