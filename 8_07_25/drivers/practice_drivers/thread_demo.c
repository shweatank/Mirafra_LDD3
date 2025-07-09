#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/delay.h>
#include<linux/init.h>
#include<linux/kthread.h>

static struct task_struct *my_thread;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("a simple thread module");

static int thread_fn(void *data)
{
	int count=0;
	printk("kthread started ...\n");
	while(!kthread_should_stop())
	{
		printk("kthread running %d\n",count++);
		msleep(1000);
	}
	printk("kthread stopping\n");
	return 0;
}

static int __init thread_init(void)
{
	printk("Kernel module loaded\n");
	my_thread=kthread_run(thread_fn,NULL,"thread_fn");
	if(IS_ERR(my_thread))
	{
		printk("Kernel failed to create my_thread\n");
		return PTR_ERR(my_thread);
	}
	return 0;
}

static void __exit thread_exit(void)
{
	if(my_thread)
	{
		kthread_stop(my_thread);
		printk("Kernel thread stopped\n");
	}
	printk("Kernel module removed\n");
}

module_init(thread_init);
module_exit(thread_exit);

