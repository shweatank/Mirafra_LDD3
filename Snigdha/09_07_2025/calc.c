#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/keyboard.h>
#include <linux/workqueue.h>
#include <linux/smp.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Snigdha");
MODULE_DESCRIPTION("Keyboard IRQ with Tasklet + Workqueue Calculator");

#define KBD_IRQ 1
#define KBD_DATA_PORT 0x60

static struct timer_list my_timer;
static struct work_struct my_work;
static struct tasklet_struct my_tasklet;


enum operation
{
    OP_NONE,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV
};

struct calc_data 
{
	int a;
	int b;
	enum operation op;
}shared_data = {.a = 10, .b=2, .op= OP_NONE};

// --- Tasklet: Addition ---
static void tasklet_func(struct tasklet_struct *t)
{
    int result = 0;

    switch(shared_data.op)
    {
	    case OP_ADD :
		    result= shared_data.a + shared_data.b;
		    pr_info("Tasklet (ADD) : %d + %d = %d\n",shared_data.a,shared_data.b,result);
		    break;
	   case OP_SUB:
		    result= shared_data.a - shared_data.b;
		    pr_info("Tasklet (SUB) : %d - %d = %d\n",shared_data.a,shared_data.b,result);
		    break;
	    default:
		    pr_info("Tasklet : Invalid operation\n");
    }

    shared_data.op = OP_NONE;
}


static void workqueue_func(struct work_struct *work)
{
    int result=0;

    switch(shared_data.op)
    {
	    case OP_MUL:
		    result= shared_data.a * shared_data.b;
		    pr_info("Workqueue (MUL) : %d * %d = %d\n",shared_data.a,shared_data.b,result);
		    break;
	    case OP_DIV:
		    result= shared_data.a / shared_data.b;
		    pr_info("Workqueue (DIV) : %d / %d = %d\n",shared_data.a,shared_data.b,result);
		    break;
	    default : 
		    pr_info("Invalid operation\n");
    }
    shared_data.op= OP_NONE;
}

static void timer_thread_handler(struct timer_list *t)
{
	if(shared_data.op == OP_MUL || shared_data.op == OP_DIV)
	{
		pr_info("Timer Interrupt : triggering workqueue\n");
		schedule_work(&my_work);
	}


	mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));
}

// --- Keyboard top half handler ---
static irqreturn_t kb_top_handler(int irq, void *dev_id)
{
    return IRQ_WAKE_THREAD;
}

// --- Keyboard bottom half (threaded handler) ---
static irqreturn_t kb_thread_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(KBD_DATA_PORT);

    switch (scancode) 
    {
        case 0x0d:
            shared_data.op= OP_ADD;
            break;
        case 0x0c:  
            shared_data.op= OP_SUB;
            break;
        case 0x09:  
            shared_data.op= OP_MUL;
            break;
        case 0x35: 
            shared_data.op= OP_DIV;
            break;
        default:
            return IRQ_HANDLED;
    }


    if(shared_data.op == OP_ADD || shared_data.op == OP_SUB)
	    tasklet_schedule(&my_tasklet);
    else
	    schedule_work(&my_work);

    return IRQ_HANDLED;
}

// --- Module Initialization ---
static int __init my_init(void)
{
    pr_info("Calculator Module Loaded\n");

    tasklet_setup(&my_tasklet, tasklet_func);

    INIT_WORK(&my_work, workqueue_func);

    if (request_threaded_irq(KBD_IRQ, kb_top_handler, kb_thread_handler,IRQF_SHARED, "kbd_irq_handler",(void *)&shared_data)) 
    {
        pr_err("Failed to register IRQ %d\n", KBD_IRQ);
        return -1;
    }
    timer_setup(&my_timer, timer_thread_handler,0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));
    return 0;
}

// --- Module Exit ---
static void __exit my_exit(void)
{
    del_timer_sync(&my_timer);
    free_irq(KBD_IRQ, (void *)&shared_data);
    tasklet_kill(&my_tasklet);
    cancel_work_sync(&my_work);

    pr_info("Calculator Module Unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

