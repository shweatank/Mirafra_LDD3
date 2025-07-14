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

#define DEVICE_NAME "mailbox_dev"
#define CLASS_NAME "vmailbox"

#define IOCTL_MAGIC 'M'
#define IOCTL_SET_ID _IOW(IOCTL_MAGIC, 1, int)

#define MAX_MSG_LEN 256
#define MAX_USERS 5

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

static struct work_struct msg_work;
static char user_message[MAX_MSG_LEN];
static char last_message[MAX_MSG_LEN];  // for procfs

// Workqueue simulating message delivery
static void simulate_delivery(struct work_struct *work) {
	msleep(2000); // delay

	snprintf(mailboxes[mailbox_id].data, MAX_MSG_LEN, "%s", user_message);
	mailboxes[mailbox_id].ready = 1;
	wake_up_interruptible(&mailbox_wq);

	if (enable_logging)
		snprintf(last_message, MAX_MSG_LEN, "Mailbox %d: %s", mailbox_id, user_message);
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
	queue_work(mailbox_wq_struct, &msg_work);

	return len;
}

static int dev_open(struct inode *inode, struct file *file) {
	return 0;
}

static int dev_release(struct inode *inode, struct file *file) {
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

// ---------------- Procfs ----------------

#define PROC_NAME "mailbox_log"

static int proc_show(struct seq_file *m, void *v) {
	seq_printf(m, "Last log: %s\n", last_message);
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
	sscanf(buf, "%d", &enable_logging);
	return count;
}

static struct kobj_attribute log_attr = __ATTR(logging, 0660, log_show, log_store);

// ---------------- Init & Exit ----------------

static int __init mailbox_init(void) {
	dev_t dev;

	if (alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME) < 0)
		return -1;

	major = MAJOR(dev);
	cdev_init(&cdev, &fops);
	cdev_add(&cdev, dev, 1);

	mailbox_class = class_create(CLASS_NAME);
	mailbox_device = device_create(mailbox_class, NULL, dev, NULL, DEVICE_NAME);

	proc_create(PROC_NAME, 0, NULL, &proc_fops);

	sysfs_kobj = kobject_create_and_add("vmailbox", kernel_kobj);
	if (sysfs_create_file(sysfs_kobj, &log_attr.attr))
		pr_warn("Failed to create sysfs file\n");

	mailbox_wq_struct = create_singlethread_workqueue("mailbox_wq");
	INIT_WORK(&msg_work, simulate_delivery);

	sema_init(&mailbox_sem, 1);

	pr_info("Virtual Mailbox loaded\n");
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
	sysfs_remove_file(sysfs_kobj, &log_attr.attr);
	kobject_put(sysfs_kobj);

	pr_info("Virtual Mailbox unloaded\n");
}

module_init(mailbox_init);
module_exit(mailbox_exit);

