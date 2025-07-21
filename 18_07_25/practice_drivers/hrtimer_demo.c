#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/hrtimer.h>
#include<linux/ktime.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("a simple hrtimer module");
static struct hrtimer timer;
static ktime_t ktime;

static enum hrtimer_restart my_hrtimer_callback(struct hrtimer *t)
{
	pr_info("hrtimer callback function called at jiffies %ld\n",jiffies);
	hrtimer_forward_now(t,ktime);
	return HRTIMER_RESTART;
}

static int __init mymod_init(void)
{
	pr_info("Initializing hrtimer...\n");
	ktime=ktime_set(1,0);
	hrtimer_init(&timer,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
	timer.function=my_hrtimer_callback;
	hrtimer_start(&timer,ktime,HRTIMER_MODE_REL);
	return 0;
}

static void __exit mymod_exit(void)
{
	pr_info("Exiting hrtimer module\n");
	hrtimer_cancel(&timer);
}

module_init(mymod_init);
module_exit(mymod_exit);
