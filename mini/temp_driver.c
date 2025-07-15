#include <linux/seq_file.h>

#include <linux/module.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/random.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/proc_fs.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Virtual Temperature Driver");

// Variables
static int current_temp = 30;
static int temp_threshold = 40;
static int alert_count = 0;
static int timer_interval_ms = 5000;
static int event_flag = 0;
static int manual_mode = 0;  // 0 = auto mode, 1 = manual mode

static struct timer_list virt_temp_timer;
static struct work_struct virt_temp_work;
static wait_queue_head_t temp_waitqueue;
static struct cdev virt_temp_cdev;
static dev_t dev_num;
static struct class *virt_temp_class;
static struct proc_dir_entry *proc_entry;
static struct kobject *virt_kobj;

// Forward declaration
static void virt_temp_work_func(struct work_struct *work);

// Timer callback: simulate temperature and check threshold
/*static void virt_temp_timer_func(struct timer_list *t)
{
    static bool was_above = false;

    // Generate random temperature 0-99 (no negatives)
    current_temp = get_random_u32() % 100;

    pr_info("[TempDrv] Timer Interrupt: New temperature = %d\n", current_temp);

    if (current_temp >= temp_threshold && !was_above) {
        alert_count++;
        event_flag = 1;
        was_above = true;

        wake_up_interruptible(&temp_waitqueue);
        pr_info("[TempDrv] ALERT: Temp=%d°C crossed Threshold=%d°C\n", current_temp, temp_threshold);
    } else if (current_temp < temp_threshold) {
        was_above = false;
    }

    // Rearm timer
    mod_timer(&virt_temp_timer, jiffies + msecs_to_jiffies(timer_interval_ms));
}*/
static void virt_temp_timer_func(struct timer_list *t)
{
    static bool was_above = false;

    if (!manual_mode) {
        current_temp = get_random_u32() % 100;
        pr_info("[TempDrv] Timer Interrupt: New temperature = %d\n", current_temp);
    } else {
        pr_info("[TempDrv] Timer Interrupt: Manual mode ON, temp = %d\n", current_temp);
    }

    if (current_temp >= temp_threshold && !was_above) {
        alert_count++;
        event_flag = 1;
        was_above = true;

        wake_up_interruptible(&temp_waitqueue);
        pr_info("[TempDrv] ALERT: Temp=%d°C crossed Threshold=%d°C\n", current_temp, temp_threshold);
    } else if (current_temp < temp_threshold) {
        was_above = false;
    }

    mod_timer(&virt_temp_timer, jiffies + msecs_to_jiffies(timer_interval_ms));
}



// Workqueue handler (dummy)
static void virt_temp_work_func(struct work_struct *work)
{
    pr_info("[TempDrv] Workqueue processing deferred temperature data\n");
}

// IOCTL commands
#define IOCTL_SET_THRESHOLD _IOW('t', 1, int)
#define IOCTL_GET_THRESHOLD _IOR('t', 2, int)
#define IOCTL_GET_ALERTS    _IOR('t', 3, int)

static long virt_temp_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int val;

    switch (cmd) {
    case IOCTL_SET_THRESHOLD:
        if (copy_from_user(&val, (int __user *)arg, sizeof(val)))
            return -EFAULT;
        temp_threshold = val;
        pr_info("[TempDrv] IOCTL: Set threshold to %d\n", temp_threshold);
        break;
    case IOCTL_GET_THRESHOLD:
        val = temp_threshold;
        if (copy_to_user((int __user *)arg, &val, sizeof(val)))
            return -EFAULT;
        break;
    case IOCTL_GET_ALERTS:
        val = alert_count;
        if (copy_to_user((int __user *)arg, &val, sizeof(val)))
            return -EFAULT;
        break;
    default:
        return -EINVAL;
    }

    return 0;
}

static int virt_temp_open(struct inode *inode, struct file *file) { return 0; }
static int virt_temp_release(struct inode *inode, struct file *file) { return 0; }

static ssize_t virt_temp_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    char kbuf[128];
    int len;

    len = snprintf(kbuf, sizeof(kbuf),
                   "Current Temp: %d\nThreshold: %d\nAlerts: %d\nTimer Interval: %d ms\n",
                   current_temp, temp_threshold, alert_count, timer_interval_ms);

    return simple_read_from_buffer(buf, count, ppos, kbuf, len);
}

static unsigned int virt_temp_poll(struct file *file, struct poll_table_struct *wait)
{
    poll_wait(file, &temp_waitqueue, wait);

    if (event_flag) {
        event_flag = 0;
        return POLLIN | POLLRDNORM;
    }

    return 0;
}

static const struct file_operations virt_temp_fops = {
    .owner = THIS_MODULE,
    .open = virt_temp_open,
    .release = virt_temp_release,
    .read = virt_temp_read,
    .unlocked_ioctl = virt_temp_ioctl,
    .poll = virt_temp_poll,
};

// Sysfs attributes for threshold and timer interval
static ssize_t threshold_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", temp_threshold);
}

static ssize_t threshold_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    int val;
    if (kstrtoint(buf, 10, &val) < 0)
        return -EINVAL;

    temp_threshold = val;
    pr_info("[TempDrv] sysfs: Set threshold to %d\n", temp_threshold);
    return count;
}

static ssize_t interval_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", timer_interval_ms);
}

static ssize_t interval_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    int val;
    if (kstrtoint(buf, 10, &val) < 0)
        return -EINVAL;

    timer_interval_ms = val;
    pr_info("[TempDrv] sysfs: Set timer interval to %d ms\n", timer_interval_ms);

    // Restart timer with new interval
    mod_timer(&virt_temp_timer, jiffies + msecs_to_jiffies(timer_interval_ms));
    return count;
}

static struct kobj_attribute threshold_attr = __ATTR(threshold, 0664, threshold_show, threshold_store);
static struct kobj_attribute interval_attr = __ATTR(timer_interval_ms, 0664, interval_show, interval_store);

static struct attribute *virt_attrs[] = {
    &threshold_attr.attr,
    &interval_attr.attr,
    NULL,
};

static struct attribute_group virt_attr_group = {
    .attrs = virt_attrs,
};

// Procfs read handler
static int proc_show(struct seq_file *m, void *v)
{
    seq_printf(m, "Current Temp: %d\nThreshold: %d\nAlerts: %d\nTimer Interval: %d ms\n",
               current_temp, temp_threshold, alert_count, timer_interval_ms);
    return 0;
}

static int proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_show, NULL);
}

static const struct proc_ops proc_fops = {
    .proc_open = proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

// Module init
static int __init virt_temp_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&dev_num, 0, 1, "virt_temp");
    if (ret < 0) {
        pr_err("[TempDrv] Failed to allocate char dev region\n");
        return ret;
    }

    cdev_init(&virt_temp_cdev, &virt_temp_fops);
    virt_temp_cdev.owner = THIS_MODULE;

    ret = cdev_add(&virt_temp_cdev, dev_num, 1);
    if (ret < 0) {
        pr_err("[TempDrv] Failed to add cdev\n");
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    virt_temp_class = class_create( "virt_temp_class");
    if (IS_ERR(virt_temp_class)) {
        pr_err("[TempDrv] Failed to create class\n");
        cdev_del(&virt_temp_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(virt_temp_class);
    }

    if (!device_create(virt_temp_class, NULL, dev_num, NULL, "virt_temp")) {
        pr_err("[TempDrv] Failed to create device\n");
        class_destroy(virt_temp_class);
        cdev_del(&virt_temp_cdev);
        unregister_chrdev_region(dev_num, 1);
        return -ENOMEM;
    }

    proc_entry = proc_create("virt_temp_status", 0, NULL, &proc_fops);
    if (!proc_entry) {
        pr_err("[TempDrv] Failed to create proc entry\n");
        device_destroy(virt_temp_class, dev_num);
        class_destroy(virt_temp_class);
        cdev_del(&virt_temp_cdev);
        unregister_chrdev_region(dev_num, 1);
        return -ENOMEM;
    }

    virt_kobj = kobject_create_and_add("virt_temp", kernel_kobj);
    if (!virt_kobj) {
        pr_err("[TempDrv] Failed to create sysfs kobject\n");
        proc_remove(proc_entry);
        device_destroy(virt_temp_class, dev_num);
        class_destroy(virt_temp_class);
        cdev_del(&virt_temp_cdev);
        unregister_chrdev_region(dev_num, 1);
        return -ENOMEM;
    }

    ret = sysfs_create_group(virt_kobj, &virt_attr_group);
    if (ret) {
        pr_err("[TempDrv] Failed to create sysfs group\n");
        kobject_put(virt_kobj);
        proc_remove(proc_entry);
        device_destroy(virt_temp_class, dev_num);
        class_destroy(virt_temp_class);
        cdev_del(&virt_temp_cdev);
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    init_waitqueue_head(&temp_waitqueue);
    INIT_WORK(&virt_temp_work, virt_temp_work_func);

    timer_setup(&virt_temp_timer, virt_temp_timer_func, 0);
    mod_timer(&virt_temp_timer, jiffies + msecs_to_jiffies(timer_interval_ms));

    pr_info("[TempDrv] Module loaded\n");
    return 0;
}

// Module exit
static void __exit virt_temp_exit(void)
{
    del_timer_sync(&virt_temp_timer);
    cancel_work_sync(&virt_temp_work);

    sysfs_remove_group(virt_kobj, &virt_attr_group);
    kobject_put(virt_kobj);
    proc_remove(proc_entry);
    device_destroy(virt_temp_class, dev_num);
    class_destroy(virt_temp_class);
    cdev_del(&virt_temp_cdev);
    unregister_chrdev_region(dev_num, 1);

    pr_info("[TempDrv] Module unloaded\n");
}

module_init(virt_temp_init);
module_exit(virt_temp_exit);

