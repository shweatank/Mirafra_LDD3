#include<linux/module.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<linux/keyboard.h>
#include<linux/workqueue.h>
#include<linux/kthread.h>
#include<linux/delay.h>
#include<linux/timer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BHARATH");
MODULE_DESCRIPTION("Timer and keyboard interrupt");

static struct timer_list my_timer;
static int irq=1;
static int major=100;
static struct task_struct *timer_thread;
static struct task_struct *key_thread;

static void tasklet_timer_fn(unsigned long);
static void tasklet_key_fn(unsigned long);

DECLARE_TASKLET(timer_tasklet,tasklet_timer_fn,0);
DECLARE_TASKLET(key_tasklet,tasklet_key_fn,0);


static void work_timer_fn(struct work_struct *work);
static void work_key_fn(struct work_struct *work);

static DECLARE_WORK(timer_work,work_timer_fn);
static DECLARE_WORK(key_work,work_key_fn);


int timer_kthread_fn(void *data){
	while(!kthread_should_stop()){
		pr_info(
