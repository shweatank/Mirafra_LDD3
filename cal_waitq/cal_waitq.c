// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <linux/mutex.h>

#define DEVICE_NAME "mywait"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba (mod by ChatGPT)");
MODULE_DESCRIPTION("Wait‑queue calc driver: add | sub | mul | div");

static dev_t dev;
static struct cdev my_cdev;
static struct class *my_class;

static DECLARE_WAIT_QUEUE_HEAD(wq);
static DEFINE_MUTEX(calc_lock);

static int flag;                      /* 0 = no result, 1 = result ready   */
static char  result_buf[64];
static size_t result_len;

/* ------------ helpers -------------------------------------------------- */

static int perform_op(const char *op, long a, long b, long *out)
{
	if (!strcmp(op, "add"))
		*out = a + b;
	else if (!strcmp(op, "sub"))
		*out = a - b;
	else if (!strcmp(op, "mul"))
		*out = a * b;
	else if (!strcmp(op, "div")) {
		if (b == 0)
			return -EINVAL;
		*out = a / b;
	} else
		return -EINVAL;

	return 0;
}

/* ------------ file operations ------------------------------------------ */

/* read() sleeps until a result is available */
static ssize_t my_read(struct file *filp, char __user *buf,
		       size_t len, loff_t *off)
{
	int ret;

	if (*off)			/* support cat(1) */
		return 0;

	/* Block until writer wakes us */
	ret = wait_event_interruptible(wq, flag);
	if (ret)
		return ret;

	mutex_lock(&calc_lock);
	if (len > result_len)
		len = result_len;

	if (copy_to_user(buf, result_buf, len))
		ret = -EFAULT;
	else {
		*off = len;		/* allow subsequent reads to return 0 */
		ret  = len;
	}

	flag = 0;			/* consume result */
	mutex_unlock(&calc_lock);
	return ret;
}

/*
 * Expects ASCII of the form:
 *     "<op> <a> <b>\n"
 * e.g. "add 3 4\n"
 */
static ssize_t my_write(struct file *filp, const char __user *buf,
			size_t len, loff_t *off)
{
	char kbuf[64];
	char op[8];
	long a, b, res;
	int ret;

	if (len >= sizeof(kbuf))
		return -EINVAL;

	if (copy_from_user(kbuf, buf, len))
		return -EFAULT;
	kbuf[len] = '\0';

	ret = sscanf(kbuf, "%7s %ld %ld", op, &a, &b);
	if (ret != 3)
		return -EINVAL;

	ret = perform_op(op, a, b, &res);
	if (ret)
		return ret;

	mutex_lock(&calc_lock);
	result_len = snprintf(result_buf, sizeof(result_buf), "%ld\n", res);
	flag = 1;
	mutex_unlock(&calc_lock);

	wake_up_interruptible(&wq);
	return len;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read  = my_read,
	.write = my_write,
};

/* ------------ init / exit ---------------------------------------------- */

static int __init waitq_init(void)
{
	int ret;

	ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
	if (ret)
		return ret;

	cdev_init(&my_cdev, &fops);
	ret = cdev_add(&my_cdev, dev, 1);
	if (ret)
		goto err_chrdev;

	my_class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(my_class)) {
		ret = PTR_ERR(my_class);
		goto err_cdev;
	}

	device_create(my_class, NULL, dev, NULL, DEVICE_NAME);
	pr_info("mywait: ready — write \"<op> a b\" then read result\n");
	return 0;

err_cdev:
	cdev_del(&my_cdev);
err_chrdev:
	unregister_chrdev_region(dev, 1);
	return ret;
}

static void __exit waitq_exit(void)
{
	device_destroy(my_class, dev);
	class_destroy(my_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("mywait: unloaded\n");
}

module_init(waitq_init);
module_exit(waitq_exit);

