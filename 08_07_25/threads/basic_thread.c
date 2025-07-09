#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/kthread.h>
#include<linux/delay.h>

static struct task_struct *task;

int thread_func(void *data)
{
	int a=(int*)data;
	a++;
	while(!kthread_should_stop())
	{
		pr_info("kernel thread running-----\n");
		msleep(1000);
	}
	pr_info("stoppinfg-----\n");
	return 0;
}
static int __init thread_init(void)
{
	pr_info("kernel module loaded\n");
	task=kthread_run(thread_func,3,"mythread");
	return 0;
}
static void __exit thread_exit(void)
{
	pr_info("kernel module removeed\n");
	kthread_stop(task);
}
MODULE_LICENSE("GPL");
module_init(thread_init);
module_exit(thread_exit);

	
