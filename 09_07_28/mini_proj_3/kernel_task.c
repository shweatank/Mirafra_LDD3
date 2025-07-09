#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kthread.h>
#include<asm/io.h>
#include<linux/timer.h>
#include<linux/uaccess.h>
#include<linux/fs.h>
#include<linux/delay.h>
#include<linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SOWMYA");
MODULE_DESCRIPTION("ALL TOPICS AT A TIME");

#define KEYBOARD_IRQ    1
#define KBD_DATA_PORT   0x60

struct my_work_data {
    struct work_struct work;
    int x, y;
};

static int a=20, b=12, result=0;

//declare tasklet_struct
static struct tasklet_struct my_tasklet;

// --- Workqueue and Work ---
static struct workqueue_struct *my_wq;
static DECLARE_DELAYED_WORK(my_work, NULL);

//thread structure
static struct task_struct *task;


//tasklet function forwared declaration
void my_tasklet_func(struct tasklet_struct *tasklet);

//timer to simulate interrupt
//static struct timer_list my_timer;

// --- Work Function ---
void my_work_func(struct work_struct *work)
{
	pr_info("workqueue:executing  the workqueue---\n");
	ssleep(1);
	pr_info("performing subtraction opetation-----\n");
	result=a-b;
	pr_info("RESULT:%d\n",result);
}
//tasklet function bottonhalf
void my_tasklet_func(struct tasklet_struct *tasklet)
{
        pr_info("tasklet:executing in softirq on cpu %u\n",smp_processor_id());
	pr_info("performing addition operation------\n");
	result=a+b;
	pr_info("RESULT:%d\n",result);
}
int my_thread_fun(void *data)
{
	//schedule the tasklet (deferred executin)
        tasklet_schedule(&my_tasklet);
        struct my_work_data *w = kmalloc(sizeof(*w), GFP_KERNEL);
        if (w) {
        w->x = a;
        w->y = b;
        INIT_WORK(&w->work, my_work_func);
        queue_work(my_wq, &w->work);
        }
	return 0;
}
//interrupt handler
irqreturn_t keyboard_irq_handler(int irq,void *dev_id)
{
	pr_info("entered into the keyboard irq----\n");
	task=kthread_run(my_thread_fun,NULL,"my_thread");
	return IRQ_HANDLED;
}

static int __init task_init(void)
{
	int result;
	result=request_irq(KEYBOARD_IRQ,keyboard_irq_handler,IRQF_SHARED,"keyboard_irq_handler",(void*)&(keyboard_irq_handler));

        if(result)
        {
                printk(KERN_ERR "keyboard_irq:cannot registr irq %d\n",KEYBOARD_IRQ);
                return result;
        }
        printk(KERN_INFO "KEYBOARD_IRQ:IRQ HANDLER REGISTERES SUCCESSFFULLY\n");

	//initialize the tasklet
        tasklet_setup(&my_tasklet,my_tasklet_func);

	  // Workqueue setup
          my_wq = alloc_workqueue("my_wq", WQ_UNBOUND, 0);

	 //initialize the timer
        //timer_setup(&my_timer,my_timer_handler,0);

        //mod_timer(&my_timer,jiffies + msecs_to_jiffies(2000));

	pr_info("kernel module installed\n");
	return 0;
}
static void __exit task_exit(void)
{
	//kill timer and tasklet
        //del_timer_sync(&my_timer);
        tasklet_kill(&my_tasklet);
        cancel_delayed_work_sync(&my_work);
	free_irq(KEYBOARD_IRQ,(void*)(keyboard_irq_handler));
}
module_init(task_init);
module_exit(task_exit);
