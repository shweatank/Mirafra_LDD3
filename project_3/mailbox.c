#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/semaphore.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/proc_fs.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/delay.h>
#include <linux/seq_file.h>
#include <linux/stdarg.h>

#define DEVICE_NAME "mailbox_dev"
#define CLASS_NAME "vmailbox"

#define IOCTL_MAGIC 'M'
#define IOCTL_SET_ID _IOW(IOCTL_MAGIC, 1, int)

#define MAX_MSG_LEN 256
#define MAX_USERS 5

#define LOG_ENTRIES 50
#define LOG_ENTRY_LEN 256

MODULE_LICENSE("GPL");

static int major;
static struct class *mailbox_class;
static struct cdev cdev;
static struct device *mailbox_device;

static DECLARE_WAIT_QUEUE_HEAD(mailbox_wq);
static struct workqueue_struct *mailbox_wq_struct;

static DEFINE_MUTEX(mailbox_mutex);
static struct semaphore mailbox_sem;

static struct kobject *sysfs_kobj;

static int mailbox_id = -1;
static int enable_logging = 1;

struct message {
    char data[MAX_MSG_LEN];
    int ready;
};

static struct message mailboxes[MAX_USERS];

// Workqueue simulating message delivery
static void simulate_delivery(struct work_struct *work);

static struct work_struct msg_work;
static char user_message[MAX_MSG_LEN];

// ---------------- Log buffer ----------------
static char log_history[LOG_ENTRIES][LOG_ENTRY_LEN];
static int log_index = 0;
static DEFINE_MUTEX(log_mutex);

// Add an entry to the log buffer
static void add_log(const char *fmt, ...) {
    va_list args;

    if (!enable_logging)
        return;

    mutex_lock(&log_mutex);

    va_start(args, fmt);
    vsnprintf(log_history[log_index], LOG_ENTRY_LEN, fmt, args);
    va_end(args);

    log_index = (log_index + 1) % LOG_ENTRIES;

    mutex_unlock(&log_mutex);
}

// Clear all logs (on init)
static void clear_logs(void) {
    int i;
    mutex_lock(&log_mutex);
    for (i = 0; i < LOG_ENTRIES; i++)
        log_history[i][0] = '\0';
    log_index = 0;
    mutex_unlock(&log_mutex);
}

// ---------------- Procfs ----------------

#define PROC_NAME "mailbox_log"

static int proc_show(struct seq_file *m, void *v) {
    int i, idx;

    mutex_lock(&log_mutex);
    idx = log_index;
    for (i = 0; i < LOG_ENTRIES; i++) {
        if (log_history[idx][0] != '\0') {
            seq_printf(m, "%s\n", log_history[idx]);
        }
        idx = (idx + 1) % LOG_ENTRIES;
    }
    mutex_unlock(&log_mutex);

    return 0;
}

static int proc_open(struct inode *inode, struct file *file) {
    return single_open(file, proc_show, NULL);
}

static const struct proc_ops proc_fops = {
    .proc_open = proc_open,
    .proc_read = seq_read,
    .proc_release = single_release,
};

// ---------------- Sysfs ----------------

static ssize_t log_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", enable_logging);
}

static ssize_t log_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    int val;
    if (kstrtoint(buf, 10, &val) == 0) {
        enable_logging = val ? 1 : 0;
        pr_info("Logging set to %d\n", enable_logging);
    }
    return count;
}

static struct kobj_attribute log_attr = __ATTR(logging, 0660, log_show, log_store);

// ---------------- Core mailbox driver ----------------

// Workqueue simulating message delivery
static void simulate_delivery(struct work_struct *work) {
    msleep(2000); // simulate delay

    snprintf(mailboxes[mailbox_id].data, MAX_MSG_LEN, "%s", user_message);
    mailboxes[mailbox_id].ready = 1;
    wake_up_interruptible(&mailbox_wq);

    add_log("Mailbox %d received message: %s", mailbox_id, user_message);
}

// ioctl to set mailbox ID
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    if (_IOC_TYPE(cmd) != IOCTL_MAGIC)
        return -EINVAL;

    if (cmd == IOCTL_SET_ID) {
        int temp_id;
        if (copy_from_user(&temp_id, (int __user *)arg, sizeof(int)))
            return -EFAULT;
        if (temp_id < 0 || temp_id >= MAX_USERS)
            return -EINVAL;

        mailbox_id = temp_id;
        pr_info("Mailbox ID set to %d\n", mailbox_id);
        add_log("Mailbox ID set to %d", mailbox_id);
        return 0;
    }

    return -EINVAL;
}

// Read message
static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    if (mailbox_id < 0)
        return -EINVAL;
    if (*off > 0)
        return 0;

    down(&mailbox_sem);
    wait_event_interruptible(mailbox_wq, mailboxes[mailbox_id].ready);
    mutex_lock(&mailbox_mutex);

    size_t msg_len = strnlen(mailboxes[mailbox_id].data, MAX_MSG_LEN);
    if (len > msg_len)
        len = msg_len;

    if (copy_to_user(buf, mailboxes[mailbox_id].data, len)) {
        mutex_unlock(&mailbox_mutex);
        up(&mailbox_sem);
        return -EFAULT;
    }

    add_log("Mailbox %d message read: %s", mailbox_id, mailboxes[mailbox_id].data);

    mailboxes[mailbox_id].ready = 0;

    mutex_unlock(&mailbox_mutex);
    up(&mailbox_sem);

    *off += len;
    return len;
}

// Write message
static ssize_t dev_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    if (mailbox_id < 0)
        return -EINVAL;

    if (copy_from_user(user_message, buf, min(len, (size_t)(MAX_MSG_LEN - 1))))
        return -EFAULT;

    user_message[min(len, (size_t)(MAX_MSG_LEN - 1))] = '\0';

    add_log("Mailbox %d message queued: %s", mailbox_id, user_message);

    queue_work(mailbox_wq_struct, &msg_work);

    return len;
}

static int dev_open(struct inode *inode, struct file *file) {
    add_log("Device opened");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file) {
    add_log("Device closed");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
    .unlocked_ioctl = dev_ioctl,
};

// ---------------- Init & Exit ----------------

static int __init mailbox_init(void) {
    dev_t dev;

    if (alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME) < 0)
        return -1;

    major = MAJOR(dev);
    cdev_init(&cdev, &fops);
    if (cdev_add(&cdev, dev, 1) < 0)
        return -1;

    mailbox_class = class_create( CLASS_NAME);
    if (IS_ERR(mailbox_class))
        return PTR_ERR(mailbox_class);

    mailbox_device = device_create(mailbox_class, NULL, dev, NULL, DEVICE_NAME);
    if (IS_ERR(mailbox_device))
        return PTR_ERR(mailbox_device);

    proc_create(PROC_NAME, 0, NULL, &proc_fops);

    sysfs_kobj = kobject_create_and_add("vmailbox", kernel_kobj);
    if (!sysfs_kobj)
        pr_warn("Failed to create sysfs kobject\n");
    else {
        if (sysfs_create_file(sysfs_kobj, &log_attr.attr))
            pr_warn("Failed to create sysfs logging file\n");
    }

    mailbox_wq_struct = create_singlethread_workqueue("mailbox_wq");
    INIT_WORK(&msg_work, simulate_delivery);

    sema_init(&mailbox_sem, 1);

    clear_logs();

    pr_info("Virtual Mailbox loaded\n");
    add_log("Virtual Mailbox module loaded");

    return 0;
}

static void __exit mailbox_exit(void) {
    dev_t dev = MKDEV(major, 0);

    flush_workqueue(mailbox_wq_struct);
    destroy_workqueue(mailbox_wq_struct);

    device_destroy(mailbox_class, dev);
    class_destroy(mailbox_class);
    cdev_del(&cdev);
    unregister_chrdev_region(dev, 1);

    remove_proc_entry(PROC_NAME, NULL);

    if (sysfs_kobj) {
        sysfs_remove_file(sysfs_kobj, &log_attr.attr);
        kobject_put(sysfs_kobj);
    }

    pr_info("Virtual Mailbox unloaded\n");
    add_log("Virtual Mailbox module unloaded");
}

module_init(mailbox_init);
module_exit(mailbox_exit);

