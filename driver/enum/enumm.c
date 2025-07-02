//kernal 
# include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "day_enum.h"               /* shared enum */

#define DEVICE_NAME "simple_char_dev"
static int  major;

/* --------‑ open/close (unchanged) ------------------------------------- */
static int dev_open(struct inode *inode, struct file *file)
{
    pr_info("%s: device opened\n", DEVICE_NAME);
    return 0;
}
static int dev_release(struct inode *inode, struct file *file)
{
    pr_info("%s: device closed\n", DEVICE_NAME);
    return 0;
}

/* --------‑ read (optional, just returns last enum) -------------------- */
static day_t last_day = SUNDAY;
static ssize_t dev_read(struct file *file,
                        char __user *buf,
                        size_t count,
                        loff_t *ppos)
{
    if (*ppos || count < sizeof(last_day))
        return 0;                       /* EOF or buffer too small */

    if (copy_to_user(buf, &last_day, sizeof(last_day)))
        return -EFAULT;

    *ppos += sizeof(last_day);
    return sizeof(last_day);
}

/* --------‑ write: receive a single enum value ------------------------- */
static ssize_t dev_write(struct file *file,
                         const char __user *buf,
                         size_t count,
                         loff_t *ppos)
{
    day_t day;

    if (count != sizeof(day))
        return -EINVAL;                 /* exact size check */

    if (copy_from_user(&day, buf, sizeof(day)))
        return -EFAULT;

    last_day = day;                     /* remember for .read() */

    switch (day) {
    case SUNDAY:    pr_info("%s: got SUNDAY\n",    DEVICE_NAME); break;
    case MONDAY:    pr_info("%s: got MONDAY\n",    DEVICE_NAME); break;
    case TUESDAY:   pr_info("%s: got TUESDAY\n",   DEVICE_NAME); break;
    case WEDNESDAY: pr_info("%s: got WEDNESDAY\n", DEVICE_NAME); break;
    case THURSDAY:  pr_info("%s: got THURSDAY\n",  DEVICE_NAME); break;
    case FRIDAY:    pr_info("%s: got FRIDAY\n",    DEVICE_NAME); break;
    case SATURDAY:  pr_info("%s: got SATURDAY\n",  DEVICE_NAME); break;
    default:        pr_warn("%s: invalid value %d\n", DEVICE_NAME, day);
                    return -EINVAL;
    }
    return sizeof(day);
}

/* --------‑ file‑ops table --------------------------------------------- */
static const struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .release = dev_release,
    .read    = dev_read,
    .write   = dev_write,
};

/* --------‑ init / exit ------------------------------------------------ */
static int __init simple_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("%s: failed to register (err=%d)\n", DEVICE_NAME, major);
        return major;
    }
    pr_info("%s: registered, major=%d\n", DEVICE_NAME, major);
    pr_info("mknod /dev/%s c %d 0\n", DEVICE_NAME, major);
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
MODULE_AUTHOR("Example");
MODULE_DESCRIPTION("Char‑dev that receives a day‑of‑week enum");

