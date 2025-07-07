// File: vmem_driver.c
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include "vmem_ioctl.h"

#define DEVICE_NAME "vmem"
#define CLASS_NAME  "vmem_class"

static int major;
static struct class *vmem_class = NULL;
static struct cdev vmem_cdev;

struct vmem_device {
    char *buffer;
    size_t size;
    size_t bytes_read;
    size_t bytes_written;
    struct mutex lock;
};

static struct vmem_device vdev;

static int vmem_open(struct inode *inode, struct file *file) {
    mutex_lock(&vdev.lock);
    if (!vdev.buffer) {
        vdev.size = 1024;
        vdev.buffer = kzalloc(vdev.size, GFP_KERNEL);
        if (!vdev.buffer) {
            mutex_unlock(&vdev.lock);
            return -ENOMEM;
        }
    }
    mutex_unlock(&vdev.lock);
    return 0;
}

static int vmem_release(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t vmem_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    ssize_t to_read;
    mutex_lock(&vdev.lock);
    if (*ppos >= vdev.size) {
        mutex_unlock(&vdev.lock);
        return 0;
    }
    to_read = min(count, vdev.size - *ppos);
    if (copy_to_user(buf, vdev.buffer + *ppos, to_read)) {
        mutex_unlock(&vdev.lock);
        return -EFAULT;
    }
    *ppos += to_read;
    vdev.bytes_read += to_read;
    mutex_unlock(&vdev.lock);
    return to_read;
}

static ssize_t vmem_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    ssize_t to_write;
    mutex_lock(&vdev.lock);
    if (*ppos >= vdev.size) {
        mutex_unlock(&vdev.lock);
        return -ENOSPC;
    }
    to_write = min(count, vdev.size - *ppos);
    if (copy_from_user(vdev.buffer + *ppos, buf, to_write)) {
        mutex_unlock(&vdev.lock);
        return -EFAULT;
    }
    *ppos += to_write;
    vdev.bytes_written += to_write;
    mutex_unlock(&vdev.lock);
    return to_write;
}

static loff_t vmem_llseek(struct file *file, loff_t offset, int whence) {
    loff_t new_pos = 0;

    mutex_lock(&vdev.lock);
    switch (whence) {
        case SEEK_SET:
            new_pos = offset;
            break;
        case SEEK_CUR:
            new_pos = file->f_pos + offset;
            break;
        case SEEK_END:
            new_pos = vdev.size + offset;
            break;
        default:
            mutex_unlock(&vdev.lock);
            return -EINVAL;
    }

    if (new_pos < 0 || new_pos > vdev.size) {
        mutex_unlock(&vdev.lock);
        return -EINVAL;
    }

    file->f_pos = new_pos;
    mutex_unlock(&vdev.lock);
    return new_pos;
}

static long vmem_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    size_t new_size;
    struct vmem_stats stats;

    switch (cmd) {
        case IOCTL_SET_MEM_SIZE:
            if (copy_from_user(&new_size, (size_t __user *)arg, sizeof(size_t)))
                return -EFAULT;
            if (new_size == 0 || new_size > 1024*1024)
                return -EINVAL;
            mutex_lock(&vdev.lock);
            kfree(vdev.buffer);
            vdev.buffer = kzalloc(new_size, GFP_KERNEL);
            if (!vdev.buffer) {
                mutex_unlock(&vdev.lock);
                return -ENOMEM;
            }
            vdev.size = new_size;
            vdev.bytes_read = vdev.bytes_written = 0;
            mutex_unlock(&vdev.lock);
            break;

        case IOCTL_CLEAR_MEM:
            mutex_lock(&vdev.lock);
            memset(vdev.buffer, 0, vdev.size);
            vdev.bytes_read = vdev.bytes_written = 0;
            mutex_unlock(&vdev.lock);
            break;

        case IOCTL_GET_STATS:
            stats.total_size = vdev.size;
            stats.bytes_read = vdev.bytes_read;
            stats.bytes_written = vdev.bytes_written;
            if (copy_to_user((struct vmem_stats __user *)arg, &stats, sizeof(stats)))
                return -EFAULT;
            break;

        default:
            return -EINVAL;
    }
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = vmem_open,
    .release = vmem_release,
    .read = vmem_read,
    .write = vmem_write,
    .llseek = vmem_llseek,
    .unlocked_ioctl = vmem_ioctl,
};

static int __init vmem_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) return major;

    vmem_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(vmem_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(vmem_class);
    }

    device_create(vmem_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    mutex_init(&vdev.lock);
    return 0;
}

static void __exit vmem_exit(void) {
    device_destroy(vmem_class, MKDEV(major, 0));
    class_destroy(vmem_class);
    unregister_chrdev(major, DEVICE_NAME);
    kfree(vdev.buffer);
}

module_init(vmem_init);
module_exit(vmem_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sunny");
MODULE_DESCRIPTION("User Controllable Virtual Memory Device");

