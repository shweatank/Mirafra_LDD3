// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/keyboard.h>
#include <linux/proc_fs.h>
#include <linux/mutex.h>
#include <asm/io.h>

#define DEVICE_NAME "emp_dev"
#define CLASS_NAME "emp_cls"
#define PROC_NAME "emp_proc"
#define STATUS_PROC_NAME "emp_status"
#define KBD_IRQ 1
#define KBD_DATA_PORT 0x60
#define IOCTL_LOGIN _IOW('a', 1, char *)

MODULE_LICENSE("GPL");

static int major;
static struct class *cls;
static struct cdev emp_cdev;
static struct proc_dir_entry *proc_entry;
static struct proc_dir_entry *status_entry;

static DECLARE_WAIT_QUEUE_HEAD(wq);
static int data_ready = 0;
static int waiting_for_input = 0;

static struct task_struct *thread_st;
static struct workqueue_struct *wq_struct;
static char user_input[128];
static char mode;
static int is_logged_in = 0;

static char result_buf[512];
static DEFINE_MUTEX(result_lock);

// Employee structure
struct emp_details {
    char name[20];
    int age;
    char dept[10];
    int salary;
};

static struct emp_details emp[6] = {
    {"revathi", 23, "SW", 10000},
    {"sumanth", 39, "HR", 20000},
    {"kusuma", 33, "IT", 20000},
    {"varsha", 34, "SW", 30000},
    {"harsha", 23, "HR", 12345},
    {"jinni", 45, "SW", 45555}
};

// TASKLET
static void emp_tasklet_fn(struct tasklet_struct *t) {
    char *name = (char *)t->data;
    int i;
    mutex_lock(&result_lock);
    result_buf[0] = '\0';

    // Remove trailing newline if present (kernel side safety)
    {
        size_t len = strlen(name);
        if (len > 0 && name[len - 1] == '\n')
            name[len - 1] = '\0';
    }

    for (i = 0; i < 6; i++) {
        if (strcmp(emp[i].name, name) == 0) {
            snprintf(result_buf, sizeof(result_buf),
                     "[Tasklet] Employee: Name=%s, Age=%d, Dept=%s, Salary=%d\n",
                     emp[i].name, emp[i].age, emp[i].dept, emp[i].salary);
            break;
        }
    }
    if (result_buf[0] == '\0')
        snprintf(result_buf, sizeof(result_buf), "Employee '%s' not found.\n", name);
    mutex_unlock(&result_lock);
}
DECLARE_TASKLET(emp_tasklet, emp_tasklet_fn);

// WORKQUEUE using delayed work
struct dept_work_data {
    struct delayed_work work;
    char name[64];
};

static struct dept_work_data *dept_work_inst;

static void dept_work_fn(struct work_struct *work) {
    struct dept_work_data *data = container_of(to_delayed_work(work), struct dept_work_data, work);
    int i;
    mutex_lock(&result_lock);
    result_buf[0] = '\0';
    {
        size_t len = strlen(data->name);
        if (len > 0 && data->name[len - 1] == '\n')
            data->name[len - 1] = '\0';
    }

    for (i = 0; i < 6; i++) {
        if (strcmp(emp[i].dept, data->name) == 0) {
            char tmp[128];
            snprintf(tmp, sizeof(tmp),
                     "[Workqueue] Dept: %s => Name=%s, Age=%d, Salary=%d\n",
                     emp[i].dept, emp[i].name, emp[i].age, emp[i].salary);
            strlcat(result_buf, tmp, sizeof(result_buf));
        }
    }
    if (result_buf[0] == '\0')
        snprintf(result_buf, sizeof(result_buf), "Department '%s' not found or no employees.\n", data->name);
    mutex_unlock(&result_lock);
}

// THREAD FUNCTION
static int handler_thread(void *arg) {
    while (!kthread_should_stop()) {
        wait_event_interruptible(wq, data_ready || kthread_should_stop());

        if (kthread_should_stop())
            break;

        if (data_ready) {
            if (mode == 'e') {
                emp_tasklet.data = (unsigned long)user_input;
                tasklet_schedule(&emp_tasklet);
            } else if (mode == 'd') {
                strncpy(dept_work_inst->name, user_input, sizeof(dept_work_inst->name) - 1);
                dept_work_inst->name[sizeof(dept_work_inst->name) - 1] = '\0';
                queue_delayed_work(wq_struct, &dept_work_inst->work, 0);
            }
            data_ready = 0;
        }
    }
    return 0;
}

// KEYBOARD HANDLER
static irqreturn_t kbd_irq_handler(int irq, void *dev_id) {
    unsigned char scancode = inb(KBD_DATA_PORT);
    char key = 0;

    // Map scancodes to 'e' and 'd' (adjust if needed)
    switch (scancode) {
        case 0x12: key = 'e'; break;  // Example scancode for 'e'
        case 0x20: key = 'd'; break;  // Example scancode for 'd'
        default: key = 0; break;
    }

    if ((key == 'e' || key == 'd') && is_logged_in && !waiting_for_input) {
        mode = key;
        waiting_for_input = 1;
        data_ready = 0;
        mutex_lock(&result_lock);
        result_buf[0] = '\0';
        mutex_unlock(&result_lock);
        wake_up_interruptible(&wq);
        pr_info("Kernel: Waiting for input for mode %c\n", mode);
    }

    return IRQ_HANDLED;
}

// IOCTL for login
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    char kbuf[32];
    if (cmd == IOCTL_LOGIN) {
        if (copy_from_user(kbuf, (char __user *)arg, sizeof(kbuf)))
            return -EFAULT;
        kbuf[sizeof(kbuf) - 1] = '\0';
        if (strcmp(kbuf, "admin") == 0) {
            is_logged_in = 1;
            pr_info("Login success\n");
            return 0;
        }
        return -EINVAL;
    }
    return -ENOTTY;
}

// procfs write
static ssize_t proc_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    if (!is_logged_in)
        return -EPERM;
    if (!waiting_for_input)
        return -EINVAL;

    if (len > sizeof(user_input) - 1)
        len = sizeof(user_input) - 1;

    if (copy_from_user(user_input, buf, len))
        return -EFAULT;

    user_input[len] = '\0';

    // Remove trailing newline if any (defensive)
    if (len > 0 && user_input[len - 1] == '\n')
        user_input[len - 1] = '\0';

    data_ready = 1;
    waiting_for_input = 0;
    wake_up_interruptible(&wq);
    return len;
}

// procfs read for result
static ssize_t proc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    ssize_t ret;
    mutex_lock(&result_lock);
    ret = simple_read_from_buffer(buf, count, ppos, result_buf, strlen(result_buf));
    mutex_unlock(&result_lock);
    return ret;
}

// procfs read for status
static ssize_t status_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    char status_msg[64];
    int len;

    if (*ppos > 0)
        return 0;

    if (waiting_for_input)
        len = snprintf(status_msg, sizeof(status_msg), "waiting\n");
    else
        len = snprintf(status_msg, sizeof(status_msg), "ready\n");

    if (copy_to_user(buf, status_msg, len))
        return -EFAULT;

    *ppos += len;
    return len;
}

static const struct proc_ops proc_fops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
};

static const struct proc_ops status_fops = {
    .proc_read = status_read,
};

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = dev_ioctl,
};

static int __init emp_init(void) {
    dev_t dev;
    int ret;

    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret)
        return ret;

    major = MAJOR(dev);
    cdev_init(&emp_cdev, &fops);
    ret = cdev_add(&emp_cdev, dev, 1);
    if (ret)
        goto unregister_chrdev;

    cls = class_create(CLASS_NAME);
    if (IS_ERR(cls)) {
        ret = PTR_ERR(cls);
        goto del_cdev;
    }

    if (IS_ERR(device_create(cls, NULL, dev, NULL, DEVICE_NAME))) {
        ret = -ENOMEM;
        goto destroy_class;
    }

    proc_entry = proc_create(PROC_NAME, 0666, NULL, &proc_fops);
    if (!proc_entry) {
        ret = -ENOMEM;
        goto destroy_device;
    }

    status_entry = proc_create(STATUS_PROC_NAME, 0444, NULL, &status_fops);
    if (!status_entry) {
        ret = -ENOMEM;
        goto remove_proc;
    }

    wq_struct = alloc_workqueue("dept_wq", WQ_UNBOUND, 0);
    if (!wq_struct) {
        ret = -ENOMEM;
        goto remove_status_proc;
    }

    dept_work_inst = kzalloc(sizeof(*dept_work_inst), GFP_KERNEL);
    if (!dept_work_inst) {
        ret = -ENOMEM;
        goto destroy_wq;
    }
    INIT_DELAYED_WORK(&dept_work_inst->work, dept_work_fn);

    thread_st = kthread_run(handler_thread, NULL, "emp_thread");
    if (IS_ERR(thread_st)) {
        ret = PTR_ERR(thread_st);
        goto free_work_inst;
    }

    ret = request_irq(KBD_IRQ, kbd_irq_handler, IRQF_SHARED, DEVICE_NAME, (void *)&emp_cdev);
    if (ret) {
        pr_err("Failed to request IRQ %d\n", KBD_IRQ);
        goto stop_thread;
    }

    pr_info("Employee Dept Kernel Module Loaded\n");
    return 0;

stop_thread:
    kthread_stop(thread_st);
free_work_inst:
    kfree(dept_work_inst);
destroy_wq:
    destroy_workqueue(wq_struct);
remove_status_proc:
    remove_proc_entry(STATUS_PROC_NAME, NULL);
remove_proc:
    remove_proc_entry(PROC_NAME, NULL);
destroy_device:
    device_destroy(cls, MKDEV(major, 0));
destroy_class:
    class_destroy(cls);
del_cdev:
    cdev_del(&emp_cdev);
unregister_chrdev:
    unregister_chrdev_region(dev, 1);
    return ret;
}

static void __exit emp_exit(void) {
    free_irq(KBD_IRQ, (void *)&emp_cdev);
    tasklet_kill(&emp_tasklet);
    cancel_delayed_work_sync(&dept_work_inst->work);
    kfree(dept_work_inst);
    destroy_workqueue(wq_struct);
    kthread_stop(thread_st);

    remove_proc_entry(STATUS_PROC_NAME, NULL);
    remove_proc_entry(PROC_NAME, NULL);
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    unregister_chrdev_region(MKDEV(major, 0), 1);
    pr_info("Employee Dept Kernel Module Unloaded\n");
}

module_init(emp_init);
module_exit(emp_exit);

