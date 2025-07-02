// iSPDX‑License‑Identifier: GPL‑2.0
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DEVICE_NAME        "simple_cal"
#define BUF_SIZE           128     /* big enough for "‑2147483648" */

static dev_t dev_no;
static struct cdev calc_cdev;
static char *kbuf;                 /* holds the ASCII result */
static size_t kbuf_len;

static int calc_open(struct inode *inode, struct file *file)
{
	/* nothing special to do; non‑exclusive */
	return 0;
}

static ssize_t calc_write(struct file *file, const char __user *ubuf,
                          size_t len, loff_t *off)
{
	/* expected format: "<op> <a> <b>\n", e.g. "add 2 3" */
	char cmd[64] = {0};
	long a, b, res = 0;
	char op[8];

	if (len >= sizeof(cmd))
		return -EINVAL;
	if (copy_from_user(cmd, ubuf, len))
		return -EFAULT;

	if (sscanf(cmd, "%7s %ld %ld", op, &a, &b) != 3)
		return -EINVAL;

	if      (!strcmp(op, "add")) res = a + b;
	else if (!strcmp(op, "sub")) res = a - b;
	else if (!strcmp(op, "mul")) res = a * b;
	else if (!strcmp(op, "div")) {
		if (b == 0) return -EINVAL;
		res = a / b;
	} else
		return -EINVAL;

	kbuf_len = scnprintf(kbuf, BUF_SIZE, "%ld\n", res);
	*off = 0;                    /* rewind for next read */
	return len;                  /* report all bytes "consumed" */
}

static ssize_t calc_read(struct file *file, char __user *ubuf,
                         size_t len, loff_t *off)
{
	if (*off >= kbuf_len)        /* EOF once result fully read */
		return 0;

	if (len > kbuf_len - *off)
		len = kbuf_len - *off;

	if (copy_to_user(ubuf, kbuf + *off, len))
		return -EFAULT;

	*off += len;
	return len;
}

static const struct file_operations calc_fops = {
	.owner  = THIS_MODULE,
	.open   = calc_open,
	.write  = calc_write,
	.read   = calc_read,
};

static int __init calc_init(void)
{
	int ret;

	/* allocate one minor dynamically */
	ret = alloc_chrdev_region(&dev_no, 0, 1, DEVICE_NAME);
	if (ret) {
		pr_err("alloc_chrdev_region failed\n");
		return ret;
	}

	cdev_init(&calc_cdev, &calc_fops);
	ret = cdev_add(&calc_cdev, dev_no, 1);
	if (ret) {
		pr_err("cdev_add failed\n");
		unregister_chrdev_region(dev_no, 1);
		return ret;
	}

	kbuf = kmalloc(BUF_SIZE, GFP_KERNEL);
	if (!kbuf) {
		cdev_del(&calc_cdev);
		unregister_chrdev_region(dev_no, 1);
		return -ENOMEM;
	}

	pr_info("/dev/%s Major %d Minor %d created\n",
	        DEVICE_NAME, MAJOR(dev_no), MINOR(dev_no));
	return 0;
}

static void __exit calc_exit(void)
{
	kfree(kbuf);
	cdev_del(&calc_cdev);
	unregister_chrdev_region(dev_no, 1);
	pr_info("/dev/%s removed\n", DEVICE_NAME);
}

module_init(calc_init);
module_exit(calc_exit);

MODULE_AUTHOR("ChatGPT demo");
MODULE_DESCRIPTION("Simple calc char device: add sub mul div");
MODULE_LICENSE("GPL");

