#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

#define KBD_IRQ 1
#define TIMER_INTERVAL_MS 5000

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sumanth");
MODULE_DESCRIPTION("Keyboard IRQ + Timer + Tasklet + Workqueue Demo");

static const int a = 10, b = 2;

static struct timer_list my_timer;
static struct tasklet_struct keyboard_tasklet;
static struct tasklet_struct timer_tasklet;
static struct workqueue_struct *my_wq;
static struct work_struct keyboard_work;
static struct work_struct timer_work;
static int dev_id = 0;

// Simple arithmetic operations
static void calc_add(void)  { pr_info("ADD: %d + %d = %d\n", a, b, a + b); }
static void calc_sub(void)  { pr_info("SUB: %d - %d = %d\n", a, b, a - b); }
static void calc_mul(void)  { pr_info("MUL: %d * %d = %d\n", a, b, a * b); }
static void calc_div(void)  { pr_info("DIV: %d / %d = %d\n", a, b, a / b); }
static void calc_mod(void)  { pr_info("MOD: %d %% %d = %d\n", a, b, a % b); }
static void calc_pow(void)  
{
	int res = 1;
	int i;
	for (i = 0; i < b; i++)
		res *= a;
	pr_info("POW: %d ^ %d = %d\n", a, b, res);
}

// IRQ handlers
static irqreturn_t kbd_top(int irq, void *dev) {
	return IRQ_WAKE_THREAD;
}

static irqreturn_t kbd_thread(int irq, void *dev)
{
	calc_add();
	tasklet_schedule(&keyboard_tasklet);
	queue_work(my_wq, &keyboard_work);
	return IRQ_HANDLED;
}

// Tasklet handlers
static void keyboard_tasklet_func(struct tasklet_struct *t)
{
	calc_sub();
}

static void timer_tasklet_func(struct tasklet_struct *t)
{
	calc_mod();
}

// Workqueue handlers
static void keyboard_work_func(struct work_struct *work)
{
	calc_mul();
}

static void timer_work_func(struct work_struct *work)
{
	calc_pow();
}

// Timer callback
static void my_timer_callback(struct timer_list *t)
{
	calc_div();
	tasklet_schedule(&timer_tasklet);
	queue_work(my_wq, &timer_work);
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_INTERVAL_MS));
}

// Init and Exit
static int __init my_module_init(void)
{
	int ret;

	pr_info("Module loading...\n");

	my_wq = alloc_workqueue("my_wq", WQ_UNBOUND | WQ_MEM_RECLAIM, 0);
	if (!my_wq)
		return -ENOMEM;

	INIT_WORK(&keyboard_work, keyboard_work_func);
	INIT_WORK(&timer_work, timer_work_func);

	tasklet_setup(&keyboard_tasklet, keyboard_tasklet_func);
	tasklet_setup(&timer_tasklet, timer_tasklet_func);

	timer_setup(&my_timer, my_timer_callback, 0);
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_INTERVAL_MS));

	ret = request_threaded_irq(KBD_IRQ, kbd_top, kbd_thread,
	                           IRQF_SHARED,
	                           "kbd_irq_thread", &dev_id);
	if (ret) {
		pr_err("Failed to request IRQ %d\n", KBD_IRQ);
		destroy_workqueue(my_wq);
		del_timer_sync(&my_timer);
		return ret;
	}

	pr_info("Module loaded successfully\n");
	return 0;
}

static void __exit my_module_exit(void)
{
	del_timer_sync(&my_timer);
	tasklet_kill(&keyboard_tasklet);
	tasklet_kill(&timer_tasklet);
	cancel_work_sync(&keyboard_work);
	cancel_work_sync(&timer_work);
	destroy_workqueue(my_wq);
	free_irq(KBD_IRQ, &dev_id);
	pr_info("Module unloaded\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

