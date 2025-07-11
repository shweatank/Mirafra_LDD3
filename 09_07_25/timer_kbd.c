#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/slab.h>

#define DEVICE_NAME "bhcalc"
#define CLASS_NAME "bhclass"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BHARATH");
MODULE_DESCRIPTION("Timer and keyboard");

static dev_t devno;
static struct class *bh_class;
static struct cdev bh_cdev;

// ---------- Shared buffer ----------
static char user_input[100];

// ---------- Thread declarations ----------
static struct task_struct *kt_thread = NULL;
static struct task_struct *kd_thread = NULL;
static int op1, op2;
static char op;

// ---------- Tasklets ----------
static void sub_tasklet(unsigned long data) {
    pr_info("Tasklet: %d - %d = %d\n", op1, op2, op1 - op2);
}
DECLARE_TASKLET(subtract_tasklet, sub_tasklet, 0);



// ---------- Workqueue ----------
static void mul_work_fn(struct work_struct *work);


static DECLARE_WORK(mul_work, mul_work_fn);


static void mul_work_fn(struct work_struct *work) {
    pr_info("Workqueue: %d * %d = %d\n", op1, op2, op1 * op2);
}


// ---------- Threads ----------
int add_kthread_fn(void *data) {
    pr_info("KThread (Add): %d + %d = %d\n", op1, op2, op1 + op2);
    return 0;
}

int div_kthread_fn(void *data) {
    if (op2 == 0)
        pr_info("KThread (Div): Divide by zero error\n");
    else
        pr_info("KThread (Div): %d / %d = %d\n", op1, op2, op1 / op2);
    return 0;
}

// ---------- Device write handler ----------
static ssize_t dev_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    int ret;

    if (len > sizeof(user_input) - 1)
        return -EINVAL;

    if (copy_from_user(user_input, buf, len))
        return -EFAULT;

    user_input[len] = '\0';
    pr_info("User input: %s\n", user_input);

    ret = sscanf(user_input, "%d%c%d", &op1, &op, &op2);
    if (ret < 2) {
        pr_info("Invalid input format\n");
        return -EINVAL;
    }

    switch (op) {
    case '+':
        kt_thread = kthread_run(add_kthread_fn, NULL, "add_thread");
        break;
    case '-':
        tasklet_schedule(&subtract_tasklet);
        break;
    case '*':
        schedule_work(&mul_work);
        break;
    case '/':
        kd_thread = kthread_run(div_kthread_fn, NULL, "div_thread");
        break;

    default:
        pr_info("Unsupported operation\n");
        break;
    }

    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = dev_write,
};

// ---------- Init ----------
static int __init bhcalc_init(void) {
    int ret;

    ret = alloc_chrdev_region(&devno, 0, 1, DEVICE_NAME);
    if (ret < 0)
        return ret;

    cdev_init(&bh_cdev, &fops);
    ret = cdev_add(&bh_cdev, devno, 1);
    if (ret < 0)
        return ret;

    bh_class = class_create(CLASS_NAME);
    if (IS_ERR(bh_class))
        return PTR_ERR(bh_class);

    device_create(bh_class, NULL, devno, NULL, DEVICE_NAME);
    pr_info("Module loaded: /dev/%s\n", DEVICE_NAME);
    return 0;
}

// ---------- Exit ----------
static void __exit bhcalc_exit(void) {
    flush_work(&mul_work);
    
    tasklet_kill(&subtract_tasklet);
    
    if (kt_thread)
        kthread_stop(kt_thread);
    if (kd_thread)
        kthread_stop(kd_thread);
    device_destroy(bh_class, devno);
    class_destroy(bh_class);
    cdev_del(&bh_cdev);
    unregister_chrdev_region(devno, 1);
    pr_info("Module unloaded\n");
}

module_init(bhcalc_init);
module_exit(bhcalc_exit);

