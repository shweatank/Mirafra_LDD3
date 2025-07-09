#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/kthread.h>
#include<linux/interrupt.h>

#define KBD_IRQ 1
static struct task_struct *my_thread;
static wait_queue_head_t wq;
static int condition=0;
static int dev_id=1;

static irqreturn_t kbd_interrupt_fun(int irq,void *dev_id)
{
	condition=1;
	wake_up(&wq);
	return IRQ_HANDLED;
}

static int thread_fn(void *data)
{
	printk("Thread started\n");
	while(!kthread_should_stop())
	{
		wait_event(wq,condition!=0);
		printk("thread: key pressed processing ... \n");
		condition=0;
	}
	printk("Thread exitting\n");
	return 0;
}

static int __init mymod_init(void)
{
	int ret;
	init_waitqueue_head(&wq);
	ret=request_irq(KBD_IRQ,kbd_interrupt_fun,IRQF_SHARED,"kbd_interrupt_fun",(void *)&dev_id);
	if(ret<0)
	{
		printk("Failed to register irq\n");
		return ret;
	}
	printk("IRQ registered successfully\n");
	my_thread=kthread_run(thread_fn,NULL,"my_thread");
	if(IS_ERR(my_thread))
	{
		free_irq(KBD_IRQ,(void *)&dev_id);
		printk("Failed to create thread\n");
		return PTR_ERR(my_thread);
	}
	printk("kthread created successfully\n");
	printk("Module loaded successfully\n");
	return 0;
}

static void __exit mymod_exit(void)
{
	kthread_stop(my_thread);
	free_irq(KBD_IRQ,(void *)&dev_id);
	printk("Module removed successfully\n");
}

module_init(mymod_init);
module_exit(mymod_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("interrupt + kthread + waitqueue");
