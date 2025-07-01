// simple_char_dev.c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>      /* for strlen() */

#define DEVICE_NAME "simple_char_dev"
#define BUF_LEN      128

static int  major;                       /* dynamically‑allocated major # */
static char device_buffer[BUF_LEN];      /* holds last data written */

/* ---------- open ------------------------------------------------------- */
static int dev_open(struct inode *inode, struct file *file)
{
        pr_info("%s: device opened\n", DEVICE_NAME);
        return 0;
}

/* ---------- release / close ------------------------------------------- */
static int dev_release(struct inode *inode, struct file *file)
{
        pr_info("%s: device closed\n", DEVICE_NAME);
        return 0;
}

/* ---------- read ------------------------------------------------------- */
static ssize_t dev_read(struct file *file,
                        char __user *user_buf,
                        size_t count,
                        loff_t *ppos)
{
        size_t bytes_avail = strlen(device_buffer);

        /* EOF if caller already consumed everything */
        if (*ppos >= bytes_avail)
                return 0;

        /* Clamp count to what we actually have left */
        if (count > bytes_avail - *ppos)
                count = bytes_avail - *ppos;

        if (copy_to_user(user_buf, device_buffer + *ppos, count))
                return -EFAULT;

        *ppos += count;  /* advance file offset */
        pr_info("%s: read %zu bytes\n", DEVICE_NAME, count);
        return count;
}

/* ---------- write ------------------------------------------------------ */
static ssize_t dev_write(struct file *file,
                         const char __user *user_buf,
                         size_t count,
                         loff_t *ppos)
{
        if (count > BUF_LEN - 1)          /* keep room for '\0' */
                count = BUF_LEN - 1;

        if (copy_from_user(device_buffer, user_buf, count))
                return -EFAULT;

        device_buffer[count] = '\0';
        pr_info("%s: wrote %zu bytes\n", DEVICE_NAME, count);
        return count;
}

/* ---------- file‑operations table -------------------------------------- */
static const struct file_operations fops = {
        .owner   = THIS_MODULE,
        .open    = dev_open,
        .release = dev_release,
        .read    = dev_read,
        .write   = dev_write,
};

/* ---------- module init / exit ----------------------------------------- */
static int __init simple_init(void)
{
        major = register_chrdev(0, DEVICE_NAME, &fops);
        if (major < 0) {
                pr_err("%s: failed to register device (err=%d)\n",
                       DEVICE_NAME, major);
                return major;
        }

        pr_info("%s: registered with major %d\n", DEVICE_NAME, major);
        pr_info("Run:  mknod /dev/%s c %d 0\n", DEVICE_NAME, major);
        return 0;
}

static void __exit simple_exit(void)
{
        unregister_chrdev(major, DEVICE_NAME);
        pr_info("%s: unregistered\n", DEVICE_NAME);
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Simple char device: open/read/write/release using printk");

