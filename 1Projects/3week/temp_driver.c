/* virtual_temp_sensor.c - A virtual temperature sensor kernel module (Updated) */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

#define DEVICE_NAME "virtual_temp"
#define CLASS_NAME  "vtemp"
#define PROCFS_NAME "virtual_temp"
#define TEMP_IOC_MAGIC  'T'
#define IOCTL_SET_THRESHOLD  _IOW(TEMP_IOC_MAGIC, 1, int)
#define IOCTL_TRIGGER_TEMP   _IO(TEMP_IOC_MAGIC, 2)
#define IOCTL_GET_CURRENT    _IOR(TEMP_IOC_MAGIC, 3, int)
#define IOCTL_STABILIZE      _IO(TEMP_IOC_MAGIC, 4)

static int major;
static struct class *temp_class;
static struct device *temp_device;
static struct cdev temp_cdev;

static struct timer_list temp_timer;
static struct tasklet_struct temp_tasklet;
static struct workqueue_struct *temp_wq;

static DECLARE_WAIT_QUEUE_HEAD(temp_wq_event);
static int event_flag = 0;

static int current_temp = 25;
static int threshold_temp = 50;

static struct proc_dir_entry *proc_entry;
static struct kobject *temp_kobj;

struct work_struct temp_work;

/* --- Timer Function (only for dmesg logging every 2 sec) --- */
static void timer_callback(struct timer_list *t)
{
    pr_info("[Timer] Temperature = %d\n", current_temp);
    mod_timer(&temp_timer, jiffies + 2 * HZ);
}

/* --- Tasklet --- */
static void tasklet_func(unsigned long data)
{
    pr_info("[Tasklet] Threshold crossed! Current Temp: %d\n", current_temp);
}

/* --- Workqueue --- */
static void work_func(struct work_struct *work)
{
    pr_info("[Workqueue] Triggered alert for threshold crossing\n");
}

/* --- File Ops --- */
static int dev_open(struct inode *inode, struct file *file) { return 0; }
static int dev_release(struct inode *inode, struct file *file) { return 0; }

static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    wait_event_interruptible(temp_wq_event, event_flag);
    event_flag = 0;
    return 0;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
        case IOCTL_SET_THRESHOLD:
            if (copy_from_user(&threshold_temp, (int __user *)arg, sizeof(int)))
                return -EFAULT;
            break;
        case IOCTL_TRIGGER_TEMP:
            current_temp++;
            if (current_temp >= threshold_temp) {
                event_flag = 1;
                wake_up_interruptible(&temp_wq_event);
                tasklet_schedule(&temp_tasklet);
                queue_work(temp_wq, &temp_work);
            }
            break;
        case IOCTL_GET_CURRENT:
            if (copy_to_user((int __user *)arg, &current_temp, sizeof(int)))
                return -EFAULT;
            break;
        case IOCTL_STABILIZE:
            current_temp = threshold_temp - 10;
            break;
        default:
            return -EINVAL;
    }
    return 0;
}

static struct file_operations fops = {
    .owner          = THIS_MODULE,
    .open           = dev_open,
    .release        = dev_release,
    .read           = dev_read,
    .unlocked_ioctl = dev_ioctl,
};

/* --- Procfs Show --- */
static int proc_show(struct seq_file *m, void *v)
{
    seq_printf(m, "Current Temp: %d\nThreshold: %d\n", current_temp, threshold_temp);
    return 0;
}
static int proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_show, NULL);
}
static struct proc_ops proc_fops = {
    .proc_open = proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

/* --- Sysfs --- */
static ssize_t threshold_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", threshold_temp);
}
static ssize_t threshold_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    sscanf(buf, "%d", &threshold_temp);
    return count;
}
static struct kobj_attribute threshold_attr = __ATTR(threshold, 0660, threshold_show, threshold_store);

static ssize_t temp_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", current_temp);
}
static struct kobj_attribute temp_attr = __ATTR(temp, 0440, temp_show, NULL);

static struct attribute *attrs[] = {
    &threshold_attr.attr,
    &temp_attr.attr,
    NULL,
};
static struct attribute_group attr_group = {
    .attrs = attrs,
};

/* --- Module Init --- */
static int __init temp_init(void)
{
    dev_t dev;
    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    major = MAJOR(dev);

    cdev_init(&temp_cdev, &fops);
    cdev_add(&temp_cdev, dev, 1);

    temp_class = class_create(CLASS_NAME);
    temp_device = device_create(temp_class, NULL, dev, NULL, DEVICE_NAME);

    proc_entry = proc_create(PROCFS_NAME, 0, NULL, &proc_fops);
    temp_kobj = kobject_create_and_add("virtual_temp", kernel_kobj);
    if (sysfs_create_group(temp_kobj, &attr_group))
        pr_err("Failed to create sysfs group\n");

    timer_setup(&temp_timer, timer_callback, 0);
    mod_timer(&temp_timer, jiffies + 2 * HZ);

    tasklet_init(&temp_tasklet, tasklet_func, 0);
    temp_wq = create_singlethread_workqueue("temp_wq");
    INIT_WORK(&temp_work, work_func);

    pr_info("[Init] Virtual temperature sensor initialized.\n");
    return 0;
}

/* --- Module Exit --- */
static void __exit temp_exit(void)
{
    del_timer(&temp_timer);
    tasklet_kill(&temp_tasklet);
    flush_workqueue(temp_wq);
    destroy_workqueue(temp_wq);

    proc_remove(proc_entry);
    sysfs_remove_group(temp_kobj, &attr_group);
    kobject_put(temp_kobj);

    device_destroy(temp_class, MKDEV(major, 0));
    class_destroy(temp_class);
    cdev_del(&temp_cdev);
    unregister_chrdev_region(MKDEV(major, 0), 1);

    pr_info("[Exit] Virtual temperature sensor removed.\n");
}

module_init(temp_init);
module_exit(temp_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("YourName");
MODULE_DESCRIPTION("Virtual Temperature Sensor Driver (Updated)");

