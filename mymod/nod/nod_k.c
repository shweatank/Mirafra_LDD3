// crash_kmod.c
//
// A minimal character device.  Writing a single byte 0x01 will invoke
// BUG() and panic the kernel.  Reading simply returns 0.
//
// Build : make -C /lib/modules/$(uname -r)/build M=$PWD modules
// Load  : sudo insmod crash_kmod.ko
// Test  : echo -n -e '\x01' | sudo tee /dev/crash_kmod
//
// GPL‑licensed for educational use only.

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVNAME "nod_k"
#define MAJOR_NUM 250        /* static major; change if already taken */

static int drv_open(struct inode *ino, struct file *f)  { return 0; }
static int drv_release(struct inode *ino, struct file *f) { return 0; }

static ssize_t drv_read(struct file *f, char __user *ubuf,
                        size_t cnt, loff_t *off)
{
        return 0;   /* nothing to read */
}

static ssize_t drv_write(struct file *f, const char __user *ubuf,
                         size_t cnt, loff_t *off)
{
        unsigned char cmd;

        if (cnt < 1 || copy_from_user(&cmd, ubuf, 1))
                return -EFAULT;

        if (cmd == 0x01) {
                pr_alert(DEVNAME ": *** deliberate BUG() triggered ***\n");
                BUG();        /* ↙️ kernel panic */
        }
        return cnt;
}

static const struct file_operations fops = {
        .owner   = THIS_MODULE,
        .open    = drv_open,
        .release = drv_release,
        .read    = drv_read,
        .write   = drv_write,
};

static int __init crash_init(void)
{
        int r = register_chrdev(MAJOR_NUM, DEVNAME, &fops);
        if (r < 0) {
                pr_err(DEVNAME ": can't register chrdev (%d)\n", r);
                return r;
        }
        pr_info(DEVNAME ": loaded (major %d)\n", MAJOR_NUM);
        pr_info("   echo -ne '\\x01' > /dev/%s  to panic\n", DEVNAME);

        /* create the device node automatically if devtmpfs is enabled */
        return 0;
}

static void __exit crash_exit(void)
{
        unregister_chrdev(MAJOR_NUM, DEVNAME);
        pr_info(DEVNAME ": unloaded\n");
}

module_init(crash_init);
module_exit(crash_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Example");
MODULE_DESCRIPTION("Intentional‐crash demo module (educational)");

