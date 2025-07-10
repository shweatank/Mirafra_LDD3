#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <asm/io.h>	
MODULE_LICENSE("GPL");
MODULE_AUTHOR("HARSHA");
MODULE_DESCRIPTION("Task with kthread + waitqueue + Keywordinterrupt");
#define KBD_DATA_PORT 0x60 
#define KBD_IRQ 1
static int dev_id=1;
static unsigned char scancode=0;
static void my_tasklet_func(struct tasklet_struct *tasklet);
static struct tasklet_struct my_tasklet;
static struct workqueue_struct *calc_wq;
static DECLARE_DELAYED_WORK(calc_work, NULL);

static struct timer_list my_timer;
static struct task_struct *my_kthread;
static int thread_flag = 0;
static int a=10,b=2;

static void my_tasklet_func1(struct tasklet_struct *tasklet);
static struct tasklet_struct my_tasklet1;

// Workqueue
static void my_work_func1(struct work_struct *work);
static struct workqueue_struct *my_wq1;
static DECLARE_WORK(my_work1, my_work_func1);

static irqreturn_t kb_top_handler(int irq,void *dev_id)
{
	pr_info("kbd_threaded:[TOP HALF] IRQ %d received\n",irq);
	scancode = inb(KBD_DATA_PORT);
	return IRQ_WAKE_THREAD;
}

static irqreturn_t kb_thread_handler(int irq,void *dev_id)
{
	pr_info("kbt_threaded:[BOTTOM HAlF] IRQ %d received\n",irq);
	pr_info("trigging the task for workqueue and tasklet\n");
        tasklet_schedule(&my_tasklet);
	queue_delayed_work(calc_wq, &calc_work, 0);
        return IRQ_HANDLED;
}

static void my_tasklet_func(struct tasklet_struct *tasklet)
{
	pr_info("Tasklet: Executing in softirq context on CPU %u\n",smp_processor_id());
	pr_info("it is called by keyword interrupt operation stareted..\n ");
	pr_info("the result is %d + %d is %d \n",a,b,a+b);
	pr_info("Tasklet keyword interrupt operation DONE..\n ");
}
static void calc_work_func(struct work_struct *work)
{
	pr_info("WorkQueue: Executing in process context\n");
	pr_info("it is called by keyword interrupt operation started..\n ");
	pr_info("the result is %d - %d is %d \n",a,b,a-b);
	pr_info("Work Queue keyword interrupt operation DONE..\n ");
}

static void timer_callback(struct timer_list *t)
{
    pr_info("Timer: Fired. Waking up kthread.\n");
    thread_flag = 1;
    wake_up_process(my_kthread);  // Wake up the sleeping thread
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));    
}

static int my_thread_fn(void *data)
{
    while (!kthread_should_stop()) 
    {
        if (thread_flag) 
	{
            thread_flag = 0;
            pr_info("Kthread is calling the workqueue and tasklet\n");
	    tasklet_schedule(&my_tasklet1);
	    queue_work(my_wq1, &my_work1);
        }
        set_current_state(TASK_INTERRUPTIBLE); // Sleep until woken by timer
        schedule();
    }
    pr_info("Kthread: Exiting\n");
    return 0;
}

static void my_tasklet_func1(struct tasklet_struct *tasklet)
{
    pr_info("Tasklet: Running in softirq context on CPU %u\n", smp_processor_id());
    pr_info("it is called by timer interrupt operation started..\n ");
    pr_info("Tasklet Result: %d * %d = %d\n", a, b, a * b);
    pr_info("Tasklet timer interrupt operation DONE..\n ");
}

static void my_work_func1(struct work_struct *work)
{
    pr_info("Workqueue: Running in process context\n");
    pr_info("it is called by timer interrupt operation started..\n ");
    pr_info("Workqueue Result: %d / %d = %d\n", a, b, a / b);
    pr_info("workqueue timer interrupt operation DONE..\n ");
}


static int __init kbd_irq_init(void)
{
	int ret;
	pr_info("kbd_threaded is loading\n");
        calc_wq = alloc_workqueue("calc_wq", WQ_UNBOUND, 0);
        INIT_DELAYED_WORK(&calc_work, calc_work_func);        
        tasklet_setup(&my_tasklet,my_tasklet_func);
	ret=request_threaded_irq(KBD_IRQ,kb_top_handler,kb_thread_handler,IRQF_SHARED,"kbd_threaded_irq",(void *)&dev_id);
	if(ret)
	{
		pr_err("kbd_threaded:failed to register IRQ %d\n",KBD_IRQ);
		return ret;
	}
	pr_info("kbd_threaded IRQ %d registered succesfully\n",KBD_IRQ);
	my_kthread = kthread_run(my_thread_fn, NULL, "my_kthread");
	tasklet_setup(&my_tasklet1, my_tasklet_func1);
	my_wq1 = alloc_workqueue("my_wq1", WQ_UNBOUND, 0);
        timer_setup(&my_timer, timer_callback, 0);

        mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));
	return 0;
}
static void __exit kbd_irq_exit(void)
{
	free_irq(KBD_IRQ,(void *)&dev_id);
	tasklet_kill(&my_tasklet);
	cancel_delayed_work_sync(&calc_work);
	if (calc_wq)
        destroy_workqueue(calc_wq);
	del_timer_sync(&my_timer);
        if (my_kthread)
        kthread_stop(my_kthread);

	tasklet_kill(&my_tasklet1);
	    flush_workqueue(my_wq1);
    destroy_workqueue(my_wq1);
	pr_info("module is unloaded\n");
}
module_init(kbd_irq_init);
module_exit(kbd_irq_exit);
